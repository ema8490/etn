RpcShadowDaemon interface {
	Ping (cid uint64) (rcid uint64, status Status)
	GetUsers (cid uint64) (rcid uint64, users []UserRecord, status Status)
	MetaGet (cid uint64, path string) (rcid uint64, filename string, lastModifiedSeconds uint32, size uint64, fileType uint32, groupset uint32, tag uint32, typeHash HashValue)
	GiveFileOpStatus (cid uint64, status Status) (rcid uint64)
	FileRead (cid uint64, path string) (rcid uint64, contents string, status Status)
	FileWriteVar (cid uint64, path string, contents string) (rcid uint64)
	DirectoryRemove (cid uint64, syscall uint32, path string) (rcid uint64, status Status)
	DirectoryCreate (cid uint64, path string, groupId uint32, labelId uint32, typeHash HashValue) (rcid uint64)
	Random (cid uint64, size uint32) (rcid uint64, data string, status Status)
}
