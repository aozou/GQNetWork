//
//  GQBaseModelObject.m
//  GQNetWorkDemo
//
//  Created by 高旗 on 16/5/27.
//  Copyright © 2016年 gaoqi. All rights reserved.
//

#import "GQBaseModelObject.h"
#import <objc/runtime.h>
#import "GQDebug.h"

NSComparator cmptr = ^(id obj1, id obj2){
    if ([obj1 integerValue] > [obj2 integerValue]) {
        return (NSComparisonResult)NSOrderedDescending;
    }
    
    if ([obj1 integerValue] < [obj2 integerValue]) {
        return (NSComparisonResult)NSOrderedAscending;
    }
    return (NSComparisonResult)NSOrderedSame;
};

static NSString * const versionAttributeMapDictionaryKey = @"versionAttributeMapDictionaryKey";

static NSString * const versionPropertykey = @"versionPropertykey";

static NSDictionary * oldPropertyVersionAndVlues = nil;

static NSInteger version = 0;

@interface GQBaseModelObject(){
    NSRecursiveLock *lock;
}

- (void)setAttributes:(NSDictionary*)dataDic;

@end

@implementation GQBaseModelObject

+ (NSDictionary *)attributeMapDictionary{
    return [[[[self class] alloc] init] propertiesAttributeMapDictionary];
}

- (NSString *)customDescription
{
    return nil;
}

- (NSData*)getArchivedData
{
    return [NSKeyedArchiver archivedDataWithRootObject:self];
}

- (NSString *)description
{
    NSMutableString *attrsDesc = [NSMutableString stringWithCapacity:100];
    NSDictionary *attrMapDic = [[self class] attributeMapDictionary];
    NSEnumerator *keyEnum = [attrMapDic keyEnumerator];
    id attributeName;
    while ((attributeName = [keyEnum nextObject])) {
        NSObject *valueObj = [self getValue:attributeName];
        if (valueObj) {
            [attrsDesc appendFormat:@" [%@=%@] ",attributeName,valueObj];
            //[valueObj release];
        }else {
            [attrsDesc appendFormat:@" [%@=nil] ",attributeName];
        }
    }
    NSString *customDesc = [self customDescription];
    NSString *desc;
    if (customDesc && [customDesc length] > 0 ) {
        desc = [NSString stringWithFormat:@"%@:{%@,%@}", [self class], attrsDesc, customDesc];
    }
    else {
        desc = [NSString stringWithFormat:@"%@:{%@}", [self class], attrsDesc];
    }
    return desc;
}



-(id)initWithDataDic:(NSDictionary*)data
{
    if (self = [super init]) {
        [self setAttributes:data];
    }
    return self;
}

- (id)copyWithZone:(NSZone *)zone
{
    id object = [[self class] allocWithZone:zone];
    NSDictionary *attrMapDic = [[self class] attributeMapDictionary];
    NSEnumerator *keyEnum = [attrMapDic keyEnumerator];
    id attributeName;
    while ((attributeName = [keyEnum nextObject])) {
        SEL getSel = NSSelectorFromString(attributeName);
        SEL sel = [object getSetterSelWithAttibuteName:attributeName];
        if ([self respondsToSelector:sel] &&
            [self respondsToSelector:getSel]) {
            NSObject *valueObj = [self getValue:attributeName];
            [object performSelectorOnMainThread:sel
                                     withObject:valueObj
                                  waitUntilDone:TRUE];
        }
    }
    return object;
}

- (id)initWithCoder:(NSCoder *)decoder
{
    if( self = [super init] ){
        NSDictionary *attrMapDic = [[self class] attributeMapDictionary];
        if (attrMapDic == nil) {
            return self;
        }
        NSMutableArray *changeOldPropertys = [[NSMutableArray alloc] initWithCapacity:0];
        NSMutableArray *changeNewPropertys = [[NSMutableArray alloc] initWithCapacity:0];
        NSArray *currentChangePropertys = [self versionChangeProperties];
        
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            oldPropertyVersionAndVlues = [decoder decodeObjectForKey:versionAttributeMapDictionaryKey];
            version = [decoder decodeIntegerForKey:versionPropertykey];
        });
        
        NSMutableArray *lastOldPropertys = [[NSMutableArray alloc] initWithArray:oldPropertyVersionAndVlues[[NSString stringWithFormat:@"%ld",version]]];
        
        if (currentChangePropertys&&[currentChangePropertys count]&&![lastOldPropertys isEqualToArray:currentChangePropertys]) {
            
            for (NSString *currentProperty in currentChangePropertys) {
                NSArray *lastCurrentPropertys = [currentProperty componentsSeparatedByString:@"->"];
                // oldChangePropertys hadn't this version changePropertys, we should use last changePropertys
                if (!lastOldPropertys) {
                    lastOldPropertys = oldPropertyVersionAndVlues[[[[oldPropertyVersionAndVlues allKeys] sortedArrayUsingComparator:cmptr] firstObject]];
                }
                
                //whether use current changePropertys
                BOOL curruntPropertysHasNotOldProperty = NO;
                
                //if never save changePropertys or,wo use current changePropertys
                if (!oldPropertyVersionAndVlues||![lastOldPropertys count]||!lastOldPropertys) {
                    curruntPropertysHasNotOldProperty = YES;
                }else{
                    int index = 0;
                    //Start from the tail ，Traverse the success of a delete one
                    for (int i = (int)[lastOldPropertys count]-1; i >= 0; i--) {
                        NSString *oldProperty = lastOldPropertys[i];
                        if ([currentProperty rangeOfString:oldProperty].location == 0&&[currentProperty rangeOfString:oldProperty].length == [oldProperty length]) {
                            NSString *lastOldProperty = [[oldProperty componentsSeparatedByString:@"->"] lastObject];
                            [changeOldPropertys addObject:lastOldProperty];
                            [changeNewPropertys addObject:[lastCurrentPropertys lastObject]];
                            [lastOldPropertys removeObject:oldProperty];
                            index--;
                            continue;
                        }else{
                            index++;
                        }
                    }
                    if (index != 0&&index == [lastOldPropertys count]) {
                        curruntPropertysHasNotOldProperty = YES;
                    }
                }
                if (curruntPropertysHasNotOldProperty) {
                    [changeOldPropertys addObject:[lastCurrentPropertys firstObject]];
                    [changeNewPropertys addObject:[lastCurrentPropertys lastObject]];
                }
            }
        }
        NSEnumerator *keyEnum = [attrMapDic keyEnumerator];
        id attributeName;
        while ((attributeName = [keyEnum nextObject])) {
            SEL sel = [self getSetterSelWithAttibuteName:attributeName];
            if ([self respondsToSelector:sel]) {
                if ([changeNewPropertys containsObject:attributeName]) {
                    attributeName = changeOldPropertys[[changeNewPropertys indexOfObject:attributeName]];
                }
                id obj = [decoder decodeObjectForKey:attributeName];
                [self performSelectorOnMainThread:sel withObject:obj waitUntilDone:[NSThread isMainThread]];
            }
        }
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
    NSDictionary *attrMapDic = [[self class] attributeMapDictionary];
    if (attrMapDic == nil) {
        return;
    }
    NSEnumerator *keyEnum = [attrMapDic keyEnumerator];
    id attributeName;
    while ((attributeName = [keyEnum nextObject])) {
        NSObject *valueObj = [self getValue:attributeName];
        if (valueObj) {
            [encoder encodeObject:valueObj forKey:attributeName];
        }
    }
    NSArray *versionChangePropertys = [self versionChangeProperties];
    if (versionChangePropertys) {
        NSMutableDictionary *newPropertyVersionAndVlues = [[NSMutableDictionary alloc] initWithDictionary:oldPropertyVersionAndVlues?oldPropertyVersionAndVlues:@{}];
        NSArray *propertys = oldPropertyVersionAndVlues[[NSString stringWithFormat:@"%ld",version]];
        //if encode dictionary not include this version change propertys, we should save new change propertys;
        if (!propertys) {
            [newPropertyVersionAndVlues setObject:versionChangePropertys forKey:[NSString stringWithFormat:@"%ld",version]];
            [encoder encodeObject:newPropertyVersionAndVlues forKey:versionAttributeMapDictionaryKey];
            [encoder encodeInteger:version forKey:versionPropertykey];
        }else{
            //if encode dictionary include this version change propertyes, we should increment our class version, and encode this version change propertys
            if (![propertys isEqualToArray:versionChangePropertys]) {
                [newPropertyVersionAndVlues setObject:versionChangePropertys forKey:[NSString stringWithFormat:@"%ld",(version+1)]];
                [encoder encodeObject:newPropertyVersionAndVlues forKey:versionAttributeMapDictionaryKey];
                [encoder encodeInteger:(version+1) forKey:versionPropertykey];
            }
        }
    }
}

#pragma mark - private methods
-(SEL)getSetterSelWithAttibuteName:(NSString*)attributeName
{
    NSString *capital = [[attributeName substringToIndex:1] uppercaseString];
    NSString *setterSelStr = [NSString stringWithFormat:@"set%@%@:",capital,[attributeName substringFromIndex:1]];
    return NSSelectorFromString(setterSelStr);
}

-(void)setAttributes:(NSDictionary*)dataDic
{
    NSDictionary *attrMapDic = [[self class] attributeMapDictionary];
    if (attrMapDic == nil) {
        return;
    }
    NSEnumerator *keyEnum = [attrMapDic keyEnumerator];
    id attributeName;
    while ((attributeName = [keyEnum nextObject])) {
        SEL sel = [self getSetterSelWithAttibuteName:attributeName];
        if ([self respondsToSelector:sel]) {
            NSString *dataDicKey = attrMapDic[attributeName];
            NSString *value = nil;
            if ([[dataDic objectForKey:dataDicKey] isKindOfClass:[NSNumber class]]) {
                value = [[dataDic objectForKey:dataDicKey] stringValue];
            }
            else if([[dataDic objectForKey:dataDicKey] isKindOfClass:[NSNull class]]){
                value = nil;
            }
            else{
                value = [dataDic objectForKey:dataDicKey];
            }
            [self performSelectorOnMainThread:sel
                                   withObject:value
                                waitUntilDone:[NSThread isMainThread]];
        }
    }
}

/*!
 * get property names of object
 */
- (NSArray*)propertyNames
{
    NSMutableArray *propertyNames = [[NSMutableArray alloc] init];
    unsigned int propertyCount = 0;
    objc_property_t *properties = class_copyPropertyList([self class], &propertyCount);
    for (unsigned int i = 0; i < propertyCount; ++i) {
        objc_property_t property = properties[i];
        const char * name = property_getName(property);
        [propertyNames addObject:[NSString stringWithUTF8String:name]];
    }
    free(properties);
    return propertyNames;
}

- (NSArray *)versionChangeProperties
{
    return nil;
}

/*!
 *	\returns a dictionary Key-Value pair by property and corresponding value.
 */
- (NSDictionary*)propertiesAndValuesDictionary
{
    NSMutableDictionary *propertiesValuesDic = [NSMutableDictionary dictionary];
    NSArray *properties = [self propertyNames];
    for (NSString *property in properties) {
        NSObject *object = [self getValue:property]?[self getValue:property]:@"";
        propertiesValuesDic[property] = object;
    }
    return propertiesValuesDic;
}

- (NSObject *)getValue:(NSString *)property{
    if (!lock) {
        lock = [[NSRecursiveLock alloc] init];
    }
    [lock lock];
    SEL getSel = NSSelectorFromString(property);
    NSObject * __unsafe_unretained valueObj = nil;
    if ([self respondsToSelector:getSel]) {
        NSMethodSignature *signature = nil;
        signature = [self methodSignatureForSelector:getSel];
        NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:signature];
        [invocation setTarget:self];
        [invocation setSelector:getSel];
        [invocation invoke];
        [invocation getReturnValue:&valueObj];
    }
    [lock unlock];
    return valueObj;
}

// default AttributeMapDictionary
- (NSDictionary*)propertiesAttributeMapDictionary
{
    NSMutableDictionary *attributeMapDictionary = [NSMutableDictionary dictionary];
    NSArray *properties = [self propertyNames];
    for (NSString *property in properties) {
        SEL getSel = NSSelectorFromString(property);
        if ([self respondsToSelector:getSel]) {
            attributeMapDictionary[property] = property;
        }
    }
    return attributeMapDictionary;
}

@end
