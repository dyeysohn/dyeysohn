/*
 * Copyright 2012 Facebook
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import <Foundation/Foundation.h>

// up-front decl's
@class FBSession;
@class FBSessionTokenCachingStrategy;

// FBSessionStatus enum
//
// Summary:
// Passed to handler block when a login call has completed
typedef enum _FBSessionState {
    // initial pre-valid invalid state
    FBSessionStateCreated               = 0,
    
    // valid session status values    
    FBSessionStateLoadedValidToken      = 1001,
    FBSessionStateLoggedIn              = 1002,  
    FBSessionStateExtendedToken         = 1003,
    
    // invalid session status values
    FBSessionStateLoggedOut             = 1,
    FBSessionStateLoginFailed           = 2, // NSError obj w/more info    
    FBSessionStateInvalidated           = 3, // "
    
} FBSessionState;

// helper macro to test for states that imply a valid session
#define FB_ISSESSIONVALIDWITHSTATE(state) (state > 1000)

typedef void (^FBSessionStatusHandler)(FBSession *session, 
                                       FBSessionState status, 
                                       NSError *error);

// FBSession class
//
// Summary:
// FBSession object is used to authenticate/authorize a user, as well
// as to manage the related access token. An FBSession object is required
// for all authenticated uses of FBRequest.
// 
// Behavior notes:
// Instances of the FBSession class notifiy of state changes in these ways; 
// a) callers of certain session* methods may provide a block to be called
// back in the course of processing the single operation (e.g. login)
// b) session instances post the "FBSessionLogin" and "FBSessionInvalid"
// notifications, which may be observed via NSNotificationCenter
// c) the object supports KVO for property changes
//
@interface FBSession : NSObject

// creating a session

// init
//
// Summary:
// Following are the descriptions of the arguments along with their 
// defaults when ommitted.
//   permissions:          - array of strings naming permissions to authorize; a 
//                         nil value indicates access to basic information; 
//                         default=nil
//   appId:                - returns a session object for the given app id; nil
//                         specifies that the appId should be pulled from the
//                         plist; default=nil
//   urlSchemeSuffix:      - suffix, used for cases where multiple iOS apps use 
//                         a single appid; nil indicates the urlSchemeSuffix
//                         should be pulled from plist; default=nil
//   tokenCachingStrategy: - policy object for fetching and storing a cached
//                         token value; when nil, the token and expiration date
//                         are stored using NSUserDefaults with the names
//                         "FBAccessTokenKey", and "FBExpirationDateKey";
//                         default=nil
//
// Behavior notes:
// For a first cut at this, we are removing the public ability
// to force an extension to an access token; instead we will implicitly do this
// when requests are made.
//
- (id)init;

- (id)initWithPermissions:(NSArray*)permissions;

- (id)initWithAppID:(NSString*)appID
           permissions:(NSArray*)permissions
       urlSchemeSuffix:(NSString*)urlSchemeSuffix
    tokenCacheStrategy:(FBSessionTokenCachingStrategy*)tokenCachingStrategy;

// instance readonly properties
@property(readonly) BOOL isValid;
@property(readonly) FBSessionState status;
@property(readonly, copy) NSString *appID;
@property(readonly, copy) NSString *urlSchemeSuffix;
@property(readonly, copy) NSString *accessToken;
@property(readonly, copy) NSDate *expirationDate;
@property(readonly, copy) NSArray *permissions;

// instance methods

// loginWithCompletionBlock logs a user on to Facebook
//
// Summary:
// Login using Facebook
//   WithCompletionBlock   - a block to call with the login result; default=nil
//
// Behavior notes:
// Login may be called zero or 1 time; calling more than once results in an exception.
//
- (void)loginWithCompletionHandler:(FBSessionStatusHandler)handler;

// invalidate
//
// Summary:
// Invalidates the local session object
//
- (void)invalidate;

// logout
//
// Summary:
// Logout invalidates the in-memory session, and clears any persisted cache
//
- (void)logout;

// handleOpenURL
//
// Summary:
// Helper method, used to provide an implementation for 
// [UIApplicationDelegate application:openUrl:*] capable of updating a session
// based on the url
//
- (BOOL)handleOpenURL:(NSURL*)url;

@end
