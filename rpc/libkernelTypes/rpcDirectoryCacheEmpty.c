#include "rpcKernel.h"

//______________________________________________________________________________
// get host information and put it in the file system
//______________________________________________________________________________
void
rpcDirectoryCacheGetHost (EtnRpcHost *host,
			  EventId eventId,
			  String *hostname,
			  String *group)
{
    // Not supported here
    BUG ();
}

//______________________________________________________________________________
/// reply for get host information
//______________________________________________________________________________
void
rpcDirectoryCacheGetHostReply (EtnRpcHost *host,
			       EventId eventId,
			       Status status)
{
    // Not supported here
    BUG ();
}

//______________________________________________________________________________
// get user information and put it in the file system
//______________________________________________________________________________
void
rpcDirectoryCacheGetUser (EtnRpcHost *host,
			  EventId eventId,
			  UserRecord userCertificate,
			  String *group)
{
    // Not supported here
    BUG ();
}

//______________________________________________________________________________
/// reply for host information
//______________________________________________________________________________
void
rpcDirectoryCacheGetUserReply (EtnRpcHost *host,
			       EventId eventId,
			       Status status)
{
    // Not supported here
    BUG ();
}

//______________________________________________________________________________
// get updated ephemeral key pair
//______________________________________________________________________________
void
rpcDirectoryCacheGenerateEphemeralKey(EtnRpcHost *host,
				      EventId eventId)
{
    // Not supported here.
    BUG ();
}

//______________________________________________________________________________
// get updated ephemeral key pair
//______________________________________________________________________________
void
rpcDirectoryCacheGenerateEphemeralKeyReply(EtnRpcHost *host,
					   EventId eventId,
					   PublicEncryptionKey ephemeralPublicKey,
					   Status status)
{
    // Not supported here.
    BUG ();
}
