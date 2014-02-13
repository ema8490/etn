#include "rpcKernel.h"

// Define an empty implementation that can later be overridden by the application.
// One implementation per source file, because gcc links at source-file granularity.

void
rpcShadowDaemonAuthUser (EtnRpcHost *host,
                         uint64_t cid,
                         uint32_t id,
                         String *name,
                         String *password,
                         String *public,
                         String *private)
{
    BUG ();
}

void
rpcShadowDaemonAuthUserReply (EtnRpcHost *host, uint64_t cid)
{
    BUG ();
}

void
rpcShadowDaemonDirectoryCreate(EtnRpcHost *host,
                               EventId     eventId,          ///< the eventId which will be replied to
                               String     *path,             ///< filename of the file
                               uint32      groupset,         ///< groupset ID 
                               uint32      tag,              ///< tag ID
                               HashValue   typeHash          ///< type hash
                              )
{
    BUG ();
}

void
rpcShadowDaemonDirectoryCreateReply(EtnRpcHost *host,
                                    EventId eventId
                                   )
{
    BUG ();
}

void
rpcShadowDaemonDirectoryRemove(EtnRpcHost *host,    ///< the connection
                               EventId     eventId, ///< the eventId which will be replied to
                               uint32      syscall, ///< syscall
                               String     *path     ///< filename of the file
)
{
    BUG ();
}

void
rpcShadowDaemonDirectoryRemoveReply(EtnRpcHost *host,    ///< the connection
                                    EventId eventId, ///< the eventId which will be replied to
                                    Status status
                                   )
{
    BUG ();
}

void
rpcShadowDaemonFileRead(EtnRpcHost *host,
                        EventId     eventId,            ///< the eventId which will be replied to
                        String     *path                ///< file descriptor to be read
                       )
{
    BUG ();
}

void
rpcShadowDaemonFileReadReply(EtnRpcHost *host,
                             EventId     eventId,
                             String     *contents,
                             Status      status
                            )
{
    BUG ();
}

void
rpcShadowDaemonFileWriteVar(EtnRpcHost *host,
                            EventId     eventId,   ///< the eventId which will be replied to
                            String     *path,      ///< file descriptor to be read
                            String     *contents   ///< contents of file after write
                )
{
    BUG ();
}

void
rpcShadowDaemonFileWriteVarReply(EtnRpcHost *host,
                                 EventId eventId   ///< the eventId which will be replied to
                )
{
    BUG ();
}

void
rpcShadowDaemonGetUsers (EtnRpcHost *host, uint64_t cid)
{
    BUG ();
}

void
rpcShadowDaemonGetUsersReply (EtnRpcHost *host, uint64_t cid, String *users)
{
    BUG ();
}

void
rpcShadowDaemonGiveFileOpStatus (EtnRpcHost *host, uint64_t cid, Status status)
{
    BUG ();
}

void
rpcShadowDaemonGiveFileOpStatusReply (EtnRpcHost *host, uint64_t rcid)
{
    BUG ();
}

void
rpcShadowDaemonMetaGet (EtnRpcHost *host, EventId eventId, String *path)
{
    BUG ();
}

void
rpcShadowDaemonMetaGetReply (EtnRpcHost *host, uint64_t cid, Status status)
{
    BUG ();
}

void
rpcShadowDaemonRandom(EtnRpcHost *host,
                      EventId eventId,    ///< the eventId which will be replied to
                      uint32 size         ///< number of random bytes need
                     )
{
    BUG ();
}

void
rpcShadowDaemonRandomReply(EtnRpcHost *host,
                           EventId     eventId,
                           String     *data,
                           Status      status
                          )
{
    BUG ();
}
