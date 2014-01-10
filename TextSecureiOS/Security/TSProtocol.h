//
//  TSProtocolManagerForThread.h
//  TextSecureiOS
//
//  Created by Christine Corbett Moran on 1/6/14.
//  Copyright (c) 2014 Open Whisper Systems. All rights reserved.
//

#import <Foundation/Foundation.h>
@class TSMessage;
@class TSThread;
@class ECKeyPair;
@class TSMessageSignal;
@class TSECKeyPair;
@class TSEncryptedWhisperMessage;
@class TSPreKeyWhisperMessage;
@class TSWhisperMessageKeys;
typedef NS_ENUM(NSInteger, TSParty) {
  TSSender=0,
  TSReceiver
};
typedef NS_ENUM(NSInteger, TSWhisperMessageType) {
  TSUnencryptedWhisperMessageType = 0,
  TSEncryptedWhisperMessageType = 1,
  TSIgnoreOnIOSWhisperMessageType=2, // on droid this is the prekey bundle message irrelevant for us
  TSPreKeyWhisperMessageType = 3
};



@protocol AxolotlPersistantStorage  <NSObject>
#warning for efficiency, past the prototyping stage we will want to group these requests
/* Axolotl Protocol variables. Persistant storage per thread. */
//RK           : 32-byte root key which gets updated by DH ratchet
+(NSData*) getRK:(TSThread*)thread;
+(void) setRK:(NSData*)key onThread:(TSThread*)thread;
//CKs, CKr     : 32-byte chain keys (used for forward-secrecy updating)
+(NSData*) getCK:(TSThread*)thread forParty:(TSParty)party;
+(void) setCK:(NSData*)key onThread:(TSThread*)thread forParty:(TSParty)party;
//DHIs, DHIr   : DH or ECDH Identity keys
+(NSData*) getDHI:(TSThread*)thread forParty:(TSParty)party;
+(void) setDHI:(NSData*)key onThread:(TSThread*)thread forParty:(TSParty)party;
//DHRs, DHRr   : DH or ECDH Ratchet keys
+(NSData*) getDHR:(TSThread*)thread forParty:(TSParty)party;
+(void) setDHR:(NSData*)key onThread:(TSThread*)thread forParty:(TSParty)party;
//Ns, Nr       : Message numbers (reset to 0 with each new ratchet)
+(NSNumber*) getN:(TSThread*)thread forParty:(TSParty)party;
+(void) setN:(NSNumber*)num onThread:(TSThread*)thread forParty:(TSParty)party;
//PNs          : Previous message numbers (# of msgs sent under prev ratchet)
+(NSNumber*)getPNs:(TSThread*)thread;
+(void)setPNs:(NSNumber*)num onThread:(TSThread*)thread;
@end




@protocol AxolotlEphemeralStorageMessagingKeys  <NSObject>
@property(nonatomic,strong) NSData* cipherKey;
@property(nonatomic,strong) NSData* macKey;
@end



@protocol AxolotlKeyAgreement <NSObject>
// all relevant database methods set inside these. only gets aloud outside of them
-(NSData*)masterKeyAlice:(TSECKeyPair*)ourIdentityKeyPair ourEphemeral:(TSECKeyPair*)ourEphemeralKeyPair theirIdentityPublicKey:(NSData*)theirIdentityPublicKey theirEphemeralPublicKey:(NSData*)theirEphemeralPublicKey;
-(NSData*)masterKeyBob:(TSECKeyPair*)ourIdentityKeyPair ourEphemeral:(TSECKeyPair*)ourEphemeralKeyPair theirIdentityPublicKey:(NSData*)theirIdentityPublicKey theirEphemeralPublicKey:(NSData*)theirEphemeralPublicKey;

-(TSWhisperMessageKeys*)initialRootKeyDerivation:(TSPreKeyWhisperMessage*)keyAgreementMessage onThread:(TSThread*)thread forParty:(TSParty) party; /* called when someone else initiazes a new session, as is indicated by the receipt of a PreKeyWhisperMessage */
-(void) newRootKeyDerivationFromNewPublicEphemeral:(NSData*)newPublicEphemeral_DHR onThread:(TSThread*)thread forParty:(TSParty)party; /* called when new message received in a session, that is not a session initialization */
-(NSData*)updateAndGetNextMessageKeyOnThread:(TSThread*)thread forParty:(TSParty)party; /* continues and existing chain */
-(TSWhisperMessageKeys*) deriveTSWhisperMessageKeysFromMessageKey:(NSData*)nextMessageKey_MK; /* just parses 64 byte NSData into 32 byte cipher and mac keys, respectively*/


@end

@protocol TSProtocol <NSObject,AxolotlKeyAgreement>

-(void) sendMessage:(TSMessage*) message onThread:(TSThread*)thread;
-(NSData*) encryptTSMessage:(TSMessage*)message onThread:(TSThread*)thread withKeys:(TSWhisperMessageKeys*)messageKeys;
-(TSMessage*) decryptMessageSignal:(TSMessageSignal*)whisperMessage;


@end

