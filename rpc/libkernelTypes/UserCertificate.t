UserCertificate struct{
		Header CertificateHeader
		UserName	string
		EncryptLongKey	PublicEncryptionKey
 		SignKey	PublicSignatureKey
}

