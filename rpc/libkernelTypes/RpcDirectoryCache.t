//______________________________________________________________________________
// gets and verifies host and user information
// if valid, puts in the filesystem
//
// upcalls are from kernel to directoryCache
// downcalls are from directoryCache to kernel
//
// downcall
//TunnelLookup(cid uint64, directoryService string)             (rcid uint64, status Status, directoryRecord DirectoryServiceRecord)
//______________________________________________________________________________

RpcDirectoryCache interface {

    // upcall
    GetHost(cid uint64, host string, group string)                (rcid uint64, status Status)

    // upcall
    GetUser(cid uint64, userCertificate UserRecord, group string) (rcid uint64, status Status)

    // downcall
    GenerateEphemeralKey(cid uint64)				  (rcid uint64, ephemeralPublicKey PublicEncryptionKey, status Status)
}

