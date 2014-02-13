DirectoryServiceRecord struct {
	Certificate        CertificateHeader
	Protocol           Uint32
	Hostname           String
	PublicKey          PublicEncryptionKey
	EphemeralPublicKey PublicEncryptionKey
	IpAddress          IpAddress
	UdpPort            UdpPort
	Padding            Uint32
}
