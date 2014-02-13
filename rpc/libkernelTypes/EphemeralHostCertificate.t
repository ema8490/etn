EphCertificate struct{
	       Header	CertificateHeader
	       HostName	string
	       EncryptLongKey	PublicEncryptionKey
	       EncryptEphemKey	PublicEncryptionKey
	       Protocol	uint8
	       Ip		IpAddress
	       Port		uint16
	       Padding		uint16
}

