//
//  GQDataRequest.m
//  GQNetWorkDemo
//
//  Created by 高旗 on 16/5/27.
//  Copyright © 2016年 gaoqi. All rights reserved.
//

#import "GQDataRequest.h"
#import "GQHttpRequestManager.h"
#import "GQNetworkTrafficManager.h"
#import "GQRequestJsonDataHandler.h"
#import "GQNetworkConsts.h"
#import "GQDataEnvironment.h"
#import "GQDebug.h"

@implementation GQDataRequest

- (void)doRequestWithParams:(NSDictionary*)params
{
    __weak typeof(self) weakSelf  = self;
    
    self.httpRequest = [[GQHTTPRequest alloc] initRequestWithParameters:params URL:self.requestUrl saveToPath:_localFilePath requestEncoding:[self getResponseEncoding] parmaterEncoding:[self getParameterEncoding] requestMethod:_requestMethod onRequestStart:^() {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (strongSelf ->_onRequestStart) {
            strongSelf->_onRequestStart(weakSelf);
        }else if (strongSelf.delegate) {
            if([strongSelf.delegate respondsToSelector:@selector(requestDidStartLoad:)]){
                [strongSelf.delegate requestDidStartLoad:strongSelf];
            }
        }
    } onProgressChanged:^(float progress) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (strongSelf->_onProgressChanged) {
            strongSelf->_onProgressChanged(weakSelf,progress);
        }else if (strongSelf.delegate) {
            if([strongSelf.delegate respondsToSelector:@selector(request:progressChanged:)]){
                [strongSelf.delegate request:strongSelf progressChanged:progress];
            }
        }
    } onRequestFinished:^(NSData *responseData) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (strongSelf->_localFilePath) {
            if (strongSelf->_onRequestFinished) {
                strongSelf->_onRequestFinished(strongSelf, nil);
            }else if (strongSelf.delegate) {
                if([strongSelf.delegate respondsToSelector:@selector(requestDidFinishLoad:mappingResult:)]){
                    [strongSelf.delegate requestDidFinishLoad:strongSelf mappingResult:nil];
                }
            }
        }else{
            [strongSelf handleResponseString:responseData];
        }
        [strongSelf showIndicator:NO];
        [strongSelf doRelease];
    } onRequestCanceled:^() {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (strongSelf->_onRequestCanceled) {
            strongSelf->_onRequestCanceled(strongSelf);
        }else if (strongSelf.delegate) {
            if([strongSelf.delegate respondsToSelector:@selector(requestDidCancelLoad:)]){
                [strongSelf.delegate requestDidCancelLoad:strongSelf];
            }
        }
        [strongSelf doRelease];
    } onRequestFailed:^(NSError *error) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        [strongSelf notifyDelegateRequestDidErrorWithError:error];
        [strongSelf showIndicator:NO];
        [strongSelf doRelease];
    }];
    
    [self.httpRequest setTimeoutInterval:[self getTimeOutInterval]];
    [self.httpRequest startRequest];
    [self showIndicator:YES];
}

- (NSDictionary*)getStaticParams
{
    return nil;
}

- (void)doRelease
{
    [super doRelease];
    self.httpRequest = nil;
}

- (GQParameterEncoding)getParameterEncoding{
    return GQURLParameterEncoding;
}

- (GQRequestMethod)getRequestMethod
{
    return GQRequestMethodGet;
}

- (void)cancelRequest
{
    [self.httpRequest cancelRequest];
    
    [self showIndicator:NO];
    GQDINFO(@"%@ request is cancled", [self class]);
}

@end
