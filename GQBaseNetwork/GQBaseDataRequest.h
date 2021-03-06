//
//  GQBaseDataRequest.h
//  GQNetWorkDemo
//
//  Created by 高旗 on 16/5/27.
//  Copyright © 2016年 gaoqi. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "GQDataCacheManager.h"
#import "GQMappingResult.h"
#import "GQNetworkConsts.h"
#import "GQRequestParameter.h"

@class GQObjectMapping;
@class GQMappingResult;

@class GQRequestDataHandler;
@class GQBaseDataRequest;
@class GQMaskActivityView;

#pragma mark -- blockTypedef  普通函数式block

typedef void (^GQRequestStart)(GQBaseDataRequest * request);//请求开始block
typedef void (^GQRequestFinished)(GQBaseDataRequest * request, GQMappingResult * result);//请求完成block
typedef void (^GQRequestCanceled)(GQBaseDataRequest * request);//请求取消block
typedef void (^GQRequestFailed)(GQBaseDataRequest * request, NSError * error);//请求失败block
typedef void (^GQProgressChanged)(GQBaseDataRequest * request, CGFloat progress);//请求进度改变block

#pragma mark -- ChainBlockTypedef 链式写法block

typedef GQBaseDataRequest *(^GQChainObjectRequest)(id value);//nsobject block
typedef GQBaseDataRequest *(^GQChainStuctRequest)(NSInteger value);   //enum block

typedef void(^GQChainBlockStartRequest)();//发送请求block

typedef GQBaseDataRequest * (^GQChainBlockRequestStart)(GQRequestStart);//请求开始block
typedef GQBaseDataRequest * (^GQChainBlockRequestFinished)(GQRequestFinished);//请求完成block
typedef GQBaseDataRequest * (^GQChainBlockRequestCanceled)(GQRequestCanceled);//请求取消block
typedef GQBaseDataRequest * (^GQChainBlockRequestFailed)(GQRequestFailed);//请求失败block
typedef GQBaseDataRequest * (^GQChainBlockProgressChanged)(GQProgressChanged);//请求进度改变block

#pragma mark -- DataRequestDelegate

@protocol DataRequestDelegate <NSObject>

@optional
- (void)requestDidStartLoad:(GQBaseDataRequest*)request;//请求开始代理
- (void)requestDidFinishLoad:(GQBaseDataRequest*)request mappingResult:(GQMappingResult *)result;//请求完成代理
- (void)requestDidCancelLoad:(GQBaseDataRequest*)request;//请求取消代理
- (void)request:(GQBaseDataRequest*)request progressChanged:(CGFloat)progress;//请求数据变化代理
- (void)request:(GQBaseDataRequest*)request didFailLoadWithError:(NSError*)error;//请求失败代理

@end

@interface GQBaseDataRequest : NSObject
{
@protected
    BOOL                _usingCacheData;
    
    GQDataCacheManagerType _cacheType;
    
    GQRequestMethod     _requestMethod;
    
    GQMaskActivityView  *_maskActivityView;
    
    //progress related
    long long           _totalData;
    
    long long           _downloadedData;
    
    CGFloat             _currentProgress;
    
    NSString            *_cancelSubject;
    
    NSInteger           _timeOutInterval;
    
    NSString            *_subRequestUrl;
    
    NSDictionary        *_parameters;
    
    NSString            *_cacheKey;
    NSString            *_localFilePath;
    
    NSString            *_keyPath;
    NSDate              *_requestStartDate;
    UIView              *_indicatorView;
    
    //callback stuffs
    GQRequestStart      _onRequestStart;
    GQRequestFinished   _onRequestFinished;
    GQRequestCanceled   _onRequestCanceled;
    GQRequestFailed     _onRequestFailed;
    GQProgressChanged   _onProgressChanged;
    
    //the finall mapping result
    GQMappingResult     *_responseResult;
    GQObjectMapping     *_mapping;
}

@property (nonatomic, strong) id<DataRequestDelegate> delegate;
@property (nonatomic, assign, readonly) BOOL loading;
@property (nonatomic, assign) CGFloat currentProgress;
@property (nonatomic, assign) GQParameterEncoding parmaterEncoding;
@property (nonatomic, copy) NSData *rawResultData;
@property (nonatomic, copy, readonly) NSString *requestUrl;
@property (nonatomic, copy, readonly) NSMutableDictionary *userInfo;
@property (nonatomic, copy, readonly) GQRequestDataHandler *requestDataHandler;

#pragma mark -- chain code methods   链式写法

+ (instancetype)prepareRequset;

/**
 *  请求地址    type : NSString
 */
@property (nonatomic, copy, readonly) GQChainObjectRequest requestUrlChain;

/**
 *  请求方法   type :枚举 GQRequestMethod
 */
@property (nonatomic, copy, readonly) GQChainStuctRequest requestMethodChain;

/**
 *  请求代理  type: self
 */
@property (nonatomic, copy, readonly) GQChainObjectRequest delegateChain;

/**
 *  拼接url  type : NSString
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest subRequestUrlChain;

/**
 *  取消请求的key   type : NSString
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest cancelSubjectChain;

/**
 *  mapping所需的key层级  type : NSString
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest keyPathChain;

/**
 *  timeOutInterval    type  NSTimeInterval
 */
@property (nonatomic, copy, readonly) GQChainStuctRequest timeOutIntervalChain;

/**
 *  具体映射的关系   type : GQObjectMapping
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest mappingChain;

/**
 *  请求参数字典   type : NSDictionary
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest parametersChain;

/**
 *  提示框所需要显示在哪个view上面  type : UIView
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest indicatorViewChain;

/**
 *  缓存key   type : NSString
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest cacheKeyChain;

/**
 *  缓存类型   type : GQDataCacheManagerType
 */
@property (nonatomic, copy , readonly) GQChainStuctRequest cacheTypeChain;

/**
 *  如果是需要把文件下载到某一个文件夹的话就传全地址  type : NSString
 */
@property (nonatomic, copy , readonly) GQChainObjectRequest localFilePathChain;

/**
 *  请求开始block    type : void (^GQRequestStart)(GQBaseDataRequest * request);
 */
@property (nonatomic, copy , readonly) GQChainBlockRequestStart onStartBlockChain;

/**
 *  请求完成block   type : void (^GQRequestFinished)(GQBaseDataRequest * request, GQMappingResult * result);
 */
@property (nonatomic, copy , readonly) GQChainBlockRequestFinished onFinishedBlockChain;

/**
 *  请求取消block   type : void (^GQRequestCanceled)(GQBaseDataRequest * request)
 */
@property (nonatomic, copy , readonly) GQChainBlockRequestCanceled onCanceledBlockChain;

/**
 *  请求失败block       type : void (^GQRequestFailed)(GQBaseDataRequest * request, NSError * error)
 */
@property (nonatomic, copy , readonly) GQChainBlockRequestFailed onFailedBlockChain;

/**
 *  请求进度改变block     type : void (^GQProgressChanged)(GQBaseDataRequest * request, CGFloat progress)
 */
@property (nonatomic, copy , readonly) GQChainBlockProgressChanged onProgressChangedBlockChain;

/**
 *  发起请求  不需要传值
 */
@property (nonatomic, copy, readonly) GQChainBlockStartRequest startRequestChain;

#pragma mark --  方法调用式写法  如果不习惯链式的block不自动提示的话就可以用下面的block写法  调用起来唯一的差别在于链式使用.调用  而下面的是方法调用
/**
 *  请求开始
 *
 *  @param onRequestStart 请求开始block
 *
 *  @return self
 */
- (instancetype)onRequestStart:(GQRequestStart)onRequestStart;
/**
 *  请求完成
 *
 *  @param onRequestFinished 请求完成block
 *
 *  @return self
 */
- (instancetype)onRequestFinished:(GQRequestFinished)onRequestFinished;
/**
 *  请求取消
 *
 *  @param onRequestCanceled 请求取消block
 *
 *  @return self
 */
- (instancetype)onRequestCanceled:(GQRequestCanceled)onRequestCanceled;
/**
 *  请求失败
 *
 *  @param onRequestFailed 请求失败block
 *
 *  @return self
 */
- (instancetype)onRequestFailed:(GQRequestFailed)onRequestFailed;
/**
 *  请求进度改变
 *
 *  @param onProgressChanged 请求进度改变block
 *
 *  @return self
 */
- (instancetype)onProgressChanged:(GQProgressChanged)onProgressChanged;
/**
 *  发起请求
 */
- (void)startRequest;

#pragma mark - class methods using delegate

/**
 *  请求方法
 *
 *  @param delegate DataRequestDelegate
 *
 *  @return  GQBaseDataRequest
 */
+ (id)requestWithDelegate:(id<DataRequestDelegate>)delegate;

/**
 *  如果想一次性配置请求参数 则配置成GQRequestParameter
 *
 *  @param parameterBody parameterBody
 *  @param delegate      DataRequestDelegate
 *
 *  @return GQBaseDataRequest
 */
+ (id)requestWithRequestParameter:(GQRequestParameter *)parameterBody
                     withDelegate:(id<DataRequestDelegate>)delegate;

/**
 *  请求方法
 *
 *  @param delegate DataRequestDelegate
 *  @param params   请求体
 *
 *  @return GQBaseDataRequest
 */
+ (id)requestWithDelegate:(id<DataRequestDelegate>)delegate
           withParameters:(NSDictionary*)params;
/**
 *  请求方法
 *
 *  @param delegate DataRequestDelegate
 *  @param subUrl   拼接url
 *
 *  @return GQBaseDataRequest
 */
+ (id)requestWithDelegate:(id<DataRequestDelegate>)delegate
        withSubRequestUrl:(NSString*)subUrl;

/**
 *  请求方法
 *
 *  @param delegate      DataRequestDelegate
 *  @param subUrl        拼接url
 *  @param cancelSubject 取消请求NSNotification的Key
 *
 *  @return GQBaseDataRequest
 */

+ (id)requestWithDelegate:(id<DataRequestDelegate>)delegate
        withSubRequestUrl:(NSString*)subUrl
        withCancelSubject:(NSString*)cancelSubject;

#pragma mark - class methods using blocks

/**
 *  不需要添加参数的请求
 *
 *  @param onFinishedBlock 请求完成block
 *  @param onFailedBlock   请求失败block
 *
 *  @return self
 */
+ (id)requestWithOnRequestFinished:(void(^)(GQBaseDataRequest *request, GQMappingResult *result))onFinishedBlock
                   onRequestFailed:(void(^)(GQBaseDataRequest *request, NSError *error))onFailedBlock;

/**
 *  添加请求体的request
 *
 *  @param params          请求体
 *  @param onFinishedBlock 请求完成block
 *  @param onFailedBlock   请求失败block
 *
 *  @return self
 */
+ (id)requestWithWithParameters:(NSDictionary*)params
              onRequestFinished:(void(^)(GQBaseDataRequest *request, GQMappingResult *result))onFinishedBlock
                onRequestFailed:(void(^)(GQBaseDataRequest *request, NSError *error))onFailedBlock;

/**
 *  添加请求体的request
 *
 *  @param params          请求体
 *  @param subUrl          拼接url
 *  @param onFinishedBlock 请求完成block
 *  @param onFailedBlock   请求失败block
 *
 *  @return self
 */
+ (id)requestWithWithParameters:(NSDictionary*)params
              withSubRequestUrl:(NSString*)subUrl
              onRequestFinished:(void(^)(GQBaseDataRequest *request, GQMappingResult *result))onFinishedBlock
                onRequestFailed:(void(^)(GQBaseDataRequest *request, NSError *error))onFailedBlock;

/**
 *  如果想一次性配置请求参数 则配置成GQRequestParameter
 *
 *  @param parameterBody          parameterBody
 *  @param onStartBlock           请求开始block
 *  @param onFinishedBlock        请求完成block
 *  @param onCanceledBlock        请求取消block
 *  @param onFailedBlock          请求失败block
 *  @param onProgressChangedBlock 请求进度block
 *
 *  @return GQBaseDataRequest
 */
+ (id)requestWithRequestParameter:(GQRequestParameter *)parameterBody
                   onRequestStart:(void(^)(GQBaseDataRequest *request))onStartBlock
                onRequestFinished:(void(^)(GQBaseDataRequest *request, GQMappingResult *result))onFinishedBlock
                onRequestCanceled:(void(^)(GQBaseDataRequest *request))onCanceledBlock
                  onRequestFailed:(void(^)(GQBaseDataRequest *request, NSError *error))onFailedBlock
                onProgressChanged:(void(^)(GQBaseDataRequest *request, CGFloat progress))onProgressChangedBlock;

#pragma mark -  file download class method using block
+ (id)requestWithParameters:(NSDictionary*)params
          withSubRequestUrl:(NSString*)subUrl
          withCancelSubject:(NSString*)cancelSubject
               withFilePath:(NSString*)localFilePath
          onRequestFinished:(void(^)(GQBaseDataRequest *request, GQMappingResult *result))onFinishedBlock
            onRequestFailed:(void(^)(GQBaseDataRequest *request, NSError *error))onFailedBlock
          onProgressChanged:(void(^)(GQBaseDataRequest *request,CGFloat progress))onProgressChangedBlock;

#pragma mark - subclass not override method
- (void)notifyRequestDidStart;
- (void)notifyRequestDidChange:(float)progress;
- (void)notifyRequestDidSuccess;
- (void)notifyRequestDidErrorWithError:(NSError*)error;
- (void)notifyRequestDidCancel;
- (void)doRelease;

#pragma mark - subclass can  override method
/*!
 * subclass can override the method, access data from responseResult and parse it to sepecfied data format
 */
- (void)processResult;
- (void)showIndicator:(BOOL)bshow;
- (void)doRequestWithParams:(NSDictionary*)params;

/**
 *  取消请求  subclass should override the method to cancel its request
 */
- (void)cancelRequest;

/**
 *  处理结果
 *
 *  @param result 请求结果
 */
- (void)handleResponseString:(NSData*)result;

/**
 *  如果需要处理缓存就覆盖这个方法  前提是要使用到了缓存
 *
 *  @param cacheData 缓存数据
 *
 *  @return 是否适用缓存数据
 */
- (BOOL)onReceivedCacheData:(NSObject*)cacheData;

/**
 *  处理下载文件   比如说 copy 取数据之类的   最终效验文件的合法性
 *
 *  @return 是否请求成功
 */
- (BOOL)processDownloadFile;

/**
 *  请求体的encoding
 *
 *  @return  parameterEncoding
 */
- (GQParameterEncoding)getParameterEncoding;               //default method is GQURLParameterEncoding

/**
 *  请求方法  默认为GET  //default method is GET
 *
 *  @return requestMethod
 */
- (GQRequestMethod)getRequestMethod;

/**
 *  请求体encode格式  默认为NSUTF8StringEncoding
 *
 *  @return  responseEncoding
 */
- (NSStringEncoding)getResponseEncoding;

/**
 *  请求url  必须重写的方法
 *
 *  @return requestUrl
 */
- (NSString*)getRequestUrl;

/**
 *  host
 *
 *  @return BaseUrl
 */
- (NSString *)getBaseUrl;

/**
 *  请求时的弹出框loading信息  默认为 “正在加载...”
 *
 *  @return loadingMessage
 */
- (NSString *)getLoadingMessage;

/**
 *  映射关系  默认为nil
 *
 *  @return mapping
 */
- (GQObjectMapping *)getMapping;

/**
 *  静态的请求参数   默认为nil
 *
 *  @return staticParams
 */
- (NSDictionary*)getStaticParams;

/**
 *  设置超时时间
 *
 *  @return timeOutInterval
 */
- (NSInteger)getTimeOutInterval;

/**
 *   从缓存string中提取字典
 *
 *  @param cachedResponse 缓存string
 *
 *  @return dic
 */
+ (NSDictionary*)getDicFromString:(NSString*)cachedResponse;

/**
 *  子类覆盖该方法修改数据解析方式  默认为json解析
 */
- (GQRequestDataHandler *)generateRequestHandler;

/**
 *  https证书二进制数据
 *
 *  @return NSData*
 */
- (NSData *)getCertificateData;

#pragma mark - 假数据方法
- (BOOL)useDumpyData;
- (NSString*)dumpyResponseString;

@end
