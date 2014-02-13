#include "rpcKernel.h"

void
rpcMinimaLTConnectionClose (EtnRpcHost *host, EventId eventId, uint32_t connectionId)
{
    BUG ();
}

void
rpcMinimaLTConnectionCloseReply (EtnRpcHost *host, EventId eventId, Status status)
{
    BUG ();
}

void
rpcMinimaLTConnectionWindowSize (EtnRpcHost *host, EventId eventId, uint32_t connectionId, uint64_t size)
{
    BUG ();
}

void
rpcMinimaLTConnectionWindowSizeReply (EtnRpcHost *host, EventId eventId, Status status)
{
    BUG ();
}

void
rpcMinimaLTDirectoryServiceLookup (EtnRpcHost *host, EventId eventId, String *hostname)
{
    BUG ();
}

void
rpcMinimaLTDirectoryServiceLookupReply (EtnRpcHost *host,
                                      EventId eventId,
                                      DirectoryServiceRecord record,
                                      Status status)
{
    BUG ();
}

void
rpcMinimaLTIpcWrite (EtnRpcHost *host, EventId eventId, String *contents)
{
    BUG ();
}

void
rpcMinimaLTIpcWriteReply (EtnRpcHost *host, EventId eventId, Status status)
{
    BUG ();
}

void
rpcMinimaLTNextTid (EtnRpcHost *host, EventId eventId, uint64_t tid, PublicEncryptionKey key)
{
    BUG ();
}

void
rpcMinimaLTNextTidReply (EtnRpcHost *host, EventId eventId, Status status)
{
    BUG ();
}

void
rpcMinimaLTPuzzle (EtnRpcHost *host, EventId eventId, Puzzle puzzle)
{
    BUG ();
}

void
rpcMinimaLTPuzzleReply (EtnRpcHost *host, EventId eventId, PuzzleSolution puzzleSolution, Status status)
{
    BUG ();
}

void
rpcMinimaLTRekeyNow (EtnRpcHost *host, EventId eventId)
{
    BUG ();
}

void
rpcMinimaLTRekeyNowReply (EtnRpcHost *host, EventId eventId, Status status)
{
    BUG ();
}

void
rpcMinimaLTConnectionCreateAuthenticateUser(EtnRpcHost         *host,
					    EventId             originatingEventId,
					    String             *serviceName,
					    HashValue           typeHash,
					    uint32              connectionId,
					    PublicEncryptionKey userPubKey,
					    Authenticator       userAuthenticator,
					    String             *writeValue)
{
    BUG ();
}

void
rpcMinimaLTConnectionCreateAuthenticateUserReply(EtnRpcHost *host,
						 EventId eventId,
						 Status  status)
{
    BUG ();
}

void
rpcMinimaLTConnectionCreate(EtnRpcHost *host,
			    EventId     originatingEventId,
			    String     *serviceName,
			    HashValue   typeHash,
			    uint32      connectionId,
			    String     *user,
			    String     *writeValue)
{
    BUG ();
}

void
rpcMinimaLTConnectionCreateReply(EtnRpcHost *host,
				 EventId eventId,
				 Status  status)
{
    BUG ();
}
