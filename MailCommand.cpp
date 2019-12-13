// MailCommand.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <CkMailMan.h>
#include <CkEmail.h>
#include <CkCert.h>
#include <CkByteData.h>
#include <CkString.h>
#include <CkStringArray.h>

/*
	-unlock	unlock-code
	-smtp	smtp-server
	-user	login
	-pwd	password
	-sub	subject
	-body	plain-text body
	-from	from
	-to	add a "to" recipient
	-cc	add a "cc" recipient
	-bcc	add a "bcc" recipient
	-log	save errors to XML log
	-port	smtp port
	-ssl	USE SSL

	-a	attachment file
	-sign
	-encrypt
	-scert	signing cert file (optional, otherwise signing certificate is located based on email address)
	-ecert	encrypting cert file  (optional, otherwise signing certificate is located based on email address)
	-zip	Zip attachments.

	-inbody	read plain-text body on stdin
	-inhtml	read html body on stdin

	-icharset	Input is this charset.
	-scharset	Send using this charset.

	-fbody 	read file for plain-text body
	-fhtml	read file for html body

	-fto     read list of email addresses from a file - one per line
	-fcc
	-fbcc

	-replyto

	-auth "login"/"plain"/..

	-rtm readTimeoutInSeconds
	
  */

int main(int argc, char* argv[])
{
    //freopen("stdout.txt","w",stdout);

    CkMailMan mailman;
    CkEmail email;
    CkString sbInCharset;
    CkString sbSendCharset;
    CkString sbAuthMethod;
    CkString sbZipFilename;
    bool hasPlainText = false;
    bool hasHtml = false;

    CkString optionsFileContents;

    // First look for the unlock code.
    // Also pickup the charset information.
    int i;
    bool unlocked = false;
    for (i=1; i<argc; i++)
	{
	if (strcmp(argv[i],"-unlock") == 0)
	    {
	    if (i < (argc-1))
		{
		unlocked = mailman.UnlockComponent(argv[i+1]);
		}
	    }
	else if (strcmp(argv[i],"-icharset") == 0)
	    {
	    if (i < (argc-1))
		{
		sbInCharset.setString(argv[i+1]);
		}
	    }
	else if (strcmp(argv[i],"-scharset") == 0)
	    {
	    if (i < (argc-1))
		{
		sbSendCharset.setString(argv[i+1]);
		}
	    }
	else if (strcmp(argv[i],"-auth") == 0)
	    {
	    if (i < (argc-1))
		{
		sbAuthMethod.setString(argv[i+1]);
		}
	    }
	}

    // Gather the options.
    bool hasErrors = false;
    bool hasSmtpServer = false;

    CkString sbXmlLog;

    for (i=1; i<argc; i++)
	{
	if (i >= (argc-1))
	    {
	    // Check for args that have no parameters here...
	    if ((strcmp(argv[i],"-sign") == 0) ||
		(strcmp(argv[i],"-encrypt") == 0) ||
		(strcmp(argv[i],"-inbody") == 0) ||
		(strcmp(argv[i],"-ssl") == 0) ||
		(strcmp(argv[i],"-inhtml") == 0))
		{
		}
	    else
		{
		const char *p = argv[i];
		if (*p != '-')
		    {
		    printf("ERROR: Missing parameter for %s\n",argv[i]);
		    hasErrors = true;
		    break;
		    }
		}
	    }
//printf("%d: [%s]\n",i,argv[i]);

	if (strcmp(argv[i],"-smtp") == 0)
	    {
	    hasSmtpServer = true;
	    mailman.put_SmtpHost(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-user") == 0)
	    {
	    mailman.put_SmtpUsername(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-inf") == 0)
	    {
	    optionsFileContents.loadFile(argv[i+1],"windows-1252");
	    i++;
	    }
	else if (strcmp(argv[i],"-unlock") == 0)
	    {
	    // We've already unlocked the component earlier, so skip the argument...
	    i++;
	    }
	else if (strcmp(argv[i],"-port") == 0)
	    {
	    int port = 25;
	    int n = sscanf(argv[i+1],"%d",&port);
	    if (n == 1)
		{
		mailman.put_SmtpPort(port);
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-rtm") == 0)
	    {
	    int readTimeout = 30;
	    int n = sscanf(argv[i+1],"%d",&readTimeout);
	    if (n == 1)
		{
		mailman.put_ReadTimeout(readTimeout);
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-pwd") == 0)
	    {
	    mailman.put_SmtpPassword(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-sub") == 0)
	    {
	    email.put_Subject(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-body") == 0)
	    {
	    hasPlainText = true;
	    email.put_Body(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-inbody") == 0)
	    {
	    }
	else if (strcmp(argv[i],"-from") == 0)
	    {
	    email.put_From(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-replyto") == 0)
	    {
	    email.put_ReplyTo(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-fto") == 0)
	    {
	    CkStringArray array;
	    if (array.LoadFromFile(argv[i+1]))
		{
		int i;
		int n = array.get_Count();
		for (i=0; i<n; i++)
		    {
		    email.AddMultipleTo(array.getString(i));
		    }
		}
	    else
		{
		hasErrors = true;
		printf("ERROR: Failed to load file: %s\n",argv[i+1]);
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-fcc") == 0)
	    {
	    CkStringArray array;
	    if (array.LoadFromFile(argv[i+1]))
		{
		int i;
		int n = array.get_Count();
		for (i=0; i<n; i++)
		    {
		    email.AddMultipleCC(array.getString(i));
		    }
		}
	    else
		{
		hasErrors = true;
		printf("ERROR: Failed to load file: %s\n",argv[i+1]);
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-fbcc") == 0)
	    {
	    CkStringArray array;
	    if (array.LoadFromFile(argv[i+1]))
		{
		int i;
		int n = array.get_Count();
		for (i=0; i<n; i++)
		    {
		    email.AddMultipleBcc(array.getString(i));
		    }
		}
	    else
		{
		hasErrors = true;
		printf("ERROR: Failed to load file: %s\n",argv[i+1]);
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-to") == 0)
	    {
	    email.AddMultipleTo(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-cc") == 0)
	    {
	    email.AddMultipleCC(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-bcc") == 0)
	    {
	    email.AddMultipleBcc(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-a") == 0)
	    {
	    CkString strContentType;
	    if (!email.AddFileAttachment(argv[i+1],strContentType))
		{
		hasErrors = true;
		printf("ERROR: Failed to add attachment: %s\n",argv[i+1]);
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-sign") == 0)
	    {
	    email.put_SendSigned(true);
	    }
	else if (strcmp(argv[i],"-encrypt") == 0)
	    {
	    email.put_SendEncrypted(true);
	    }
	else if (strcmp(argv[i],"-scert") == 0)
	    {
	    CkCert cert;
	    if (!cert.LoadFromFile(argv[i+1]))
		{
		hasErrors = true;
		printf("ERROR: Failed to load signing cert: %s\n",argv[i+1]);
		}
	    else
		{
		if (!email.SetSigningCert(cert))
		    {
		    hasErrors = true;
		    printf("ERROR: Failed to set signing cert: %s\n",argv[i+1]);
		    }
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-ecert") == 0)
	    {
	    CkCert cert;
	    if (!cert.LoadFromFile(argv[i+1]))
		{
		hasErrors = true;
		printf("%s\n",email.lastErrorText());
		printf("ERROR: Failed to load encrypting cert: %s\n",argv[i+1]);
		}
	    else
		{
		if (!email.SetEncryptCert(cert))
		    {
		    hasErrors = true;
		    printf("%s\n",email.lastErrorText());
		    printf("ERROR: Failed to set encrypt cert: %s\n",argv[i+1]);
		    }
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-ssl") == 0)
	    {
	    mailman.put_SmtpSsl(true);
	    }
	else if (strcmp(argv[i],"-starttls") == 0)
	    {
	    mailman.put_StartTLS(true);
	    }
	else if (strcmp(argv[i],"-zip") == 0)
	    {
	    sbZipFilename.setString(argv[i+1]);
	    i++;
	    }
	else if (strcmp(argv[i],"-fbody") == 0)
	    {
	    hasPlainText = true;
	    CkByteData data;
	    if (!data.loadFile(argv[i+1]))
		{
		hasErrors = true;
		printf("ERROR: Failed to read %s\n",argv[i+1]);
		}
	    else
		{
		const char *charset = "us-ascii";

		if (sbInCharset.getNumChars())
		    {
		    charset = sbInCharset.getString();
		    }

		if (!email.SetMbPlainTextBody(charset,data))
		    {
		    hasErrors = true;
		    printf("ERROR: Failed to set plain text body.\n");
		    }
		}
	    i++;
	    }
	else if (strcmp(argv[i],"-log") == 0)
	    {
	    sbXmlLog.clear();
	    sbXmlLog.append(argv[i+1]);
	    i++;
	    }

	}


    // ------------------------------------------------------------------------------
    // Process options from -inf:
    if (optionsFileContents.getSizeAnsi())
	{
	CkStringArray *sa = optionsFileContents.split2(" \t",true,false,false);
	if (sa)
	    {
	    int numOpts = sa->get_Count();
	    for (i=0; i<numOpts; i++)
		{
		CkString optStr;
		sa->GetString(i,optStr);
		if (optStr.getSizeAnsi() == 0) continue;

		if (optStr.beginsWith("\""))
		    {
		    optStr.replaceFirstOccurance("\"","");
		    optStr.shorten(1);
		    }

		CkString optVal;
		sa->GetString(i+1,optVal);
		if (optVal.beginsWith("\""))
		    {
		    optVal.replaceFirstOccurance("\"","");
		    optVal.shorten(1);
		    }

		const char *opt = optStr.getString();
		if (!opt) continue;

		if (strcmp(opt,"-unlock") == 0)
		    {
		    if (i < (numOpts-1))
			{
			unlocked = mailman.UnlockComponent(optVal.getString());
			}
		    }
		else if (strcmp(opt,"-icharset") == 0)
		    {
		    if (i < (numOpts-1))
			{
			sbInCharset.setString(optVal.getString());
			}
		    }
		else if (strcmp(opt,"-scharset") == 0)
		    {
		    if (i < (numOpts-1))
			{
			sbSendCharset.setString(optVal.getString());
			}
		    }
		else if (strcmp(opt,"-auth") == 0)
		    {
		    if (i < (numOpts-1))
			{
			sbAuthMethod.setString(optVal.getString());
			}
		    }

		}

	    for (i=0; i<numOpts; i++)
		{

		CkString optStr;
		sa->GetString(i,optStr);
		if (optStr.getSizeAnsi() == 0) continue;

		if (optStr.beginsWith("\""))
		    {
		    optStr.replaceFirstOccurance("\"","");
		    optStr.shorten(1);
		    }

		CkString optVal;
		sa->GetString(i+1,optVal);
		if (optVal.beginsWith("\""))
		    {
		    optVal.replaceFirstOccurance("\"","");
		    optVal.shorten(1);
		    }

		const char *opt = optStr.getString();
		if (!opt) continue;




		if (i >= (numOpts-1))
		    {
		    // Check for args that have no parameters here...
		    if ((strcmp(opt,"-sign") == 0) ||
			(strcmp(opt,"-encrypt") == 0) ||
			(strcmp(opt,"-inbody") == 0) ||
			(strcmp(opt,"-ssl") == 0) ||
			(strcmp(opt,"-inhtml") == 0))
			{
			}
		    else
			{
			const char *p = opt;
			if (*p != '-')
			    {
			    printf("ERROR: Missing parameter for %s\n",opt);
			    hasErrors = true;
			    break;
			    }
			}
		    }
	//printf("%d: [%s]\n",i,opt);

		if (strcmp(opt,"-smtp") == 0)
		    {
		    hasSmtpServer = true;
		    mailman.put_SmtpHost(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-user") == 0)
		    {
		    mailman.put_SmtpUsername(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-inf") == 0)
		    {
		    optionsFileContents.loadFile(optVal.getString(),"windows-1252");
		    i++;
		    }
		else if (strcmp(opt,"-unlock") == 0)
		    {
		    // We've already unlocked the component earlier, so skip the argument...
		    i++;
		    }
		else if (strcmp(opt,"-port") == 0)
		    {
		    int port = 25;
		    int n = sscanf(optVal.getString(),"%d",&port);
		    if (n == 1)
			{
			mailman.put_SmtpPort(port);
			}
		    i++;
		    }
		else if (strcmp(opt,"-pwd") == 0)
		    {
		    mailman.put_SmtpPassword(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-sub") == 0)
		    {
		    email.put_Subject(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-body") == 0)
		    {
		    hasPlainText = true;
		    email.put_Body(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-inbody") == 0)
		    {
		    }
		else if (strcmp(opt,"-from") == 0)
		    {
		    email.put_From(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-replyto") == 0)
		    {
		    email.put_ReplyTo(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-fto") == 0)
		    {
		    CkStringArray array;
		    if (array.LoadFromFile(optVal.getString()))
			{
			int i;
			int n = array.get_Count();
			for (i=0; i<n; i++)
			    {
			    email.AddMultipleTo(array.getString(i));
			    }
			}
		    else
			{
			hasErrors = true;
			printf("ERROR: Failed to load file: %s\n",optVal.getString());
			}
		    i++;
		    }
		else if (strcmp(opt,"-fcc") == 0)
		    {
		    CkStringArray array;
		    if (array.LoadFromFile(optVal.getString()))
			{
			int i;
			int n = array.get_Count();
			for (i=0; i<n; i++)
			    {
			    email.AddMultipleCC(array.getString(i));
			    }
			}
		    else
			{
			hasErrors = true;
			printf("ERROR: Failed to load file: %s\n",optVal.getString());
			}
		    i++;
		    }
		else if (strcmp(opt,"-fbcc") == 0)
		    {
		    CkStringArray array;
		    if (array.LoadFromFile(optVal.getString()))
			{
			int i;
			int n = array.get_Count();
			for (i=0; i<n; i++)
			    {
			    email.AddMultipleBcc(array.getString(i));
			    }
			}
		    else
			{
			hasErrors = true;
			printf("ERROR: Failed to load file: %s\n",optVal.getString());
			}
		    i++;
		    }
		else if (strcmp(opt,"-to") == 0)
		    {
		    email.AddMultipleTo(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-cc") == 0)
		    {
		    email.AddMultipleCC(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-bcc") == 0)
		    {
		    email.AddMultipleBcc(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-a") == 0)
		    {
		    CkString strContentType;
		    if (!email.AddFileAttachment(optVal.getString(),strContentType))
			{
			hasErrors = true;
			printf("ERROR: Failed to add attachment: %s\n",optVal.getString());
			}
		    i++;
		    }
		else if (strcmp(opt,"-sign") == 0)
		    {
		    email.put_SendSigned(true);
		    }
		else if (strcmp(opt,"-encrypt") == 0)
		    {
		    email.put_SendEncrypted(true);
		    }
		else if (strcmp(opt,"-scert") == 0)
		    {
		    CkCert cert;
		    if (!cert.LoadFromFile(optVal.getString()))
			{
			hasErrors = true;
			printf("ERROR: Failed to load signing cert: %s\n",optVal.getString());
			}
		    else
			{
			email.SetSigningCert(cert);
			}
		    i++;
		    }
		else if (strcmp(opt,"-ecert") == 0)
		    {
		    CkCert cert;
		    if (!cert.LoadFromFile(optVal.getString()))
			{
			hasErrors = true;
			printf("ERROR: Failed to load encrypting cert: %s\n",optVal.getString());
			}
		    else
			{
			email.SetEncryptCert(cert);
			}
		    i++;
		    }
		else if (strcmp(opt,"-ssl") == 0)
		    {
		    mailman.put_SmtpSsl(true);
		    }
		else if (strcmp(opt,"-starttls") == 0)
		    {
		    mailman.put_StartTLS(true);
		    }
		else if (strcmp(opt,"-zip") == 0)
		    {
		    sbZipFilename.setString(optVal.getString());
		    i++;
		    }
		else if (strcmp(opt,"-fbody") == 0)
		    {
		    hasPlainText = true;
		    CkByteData data;
		    if (!data.loadFile(optVal.getString()))
			{
			hasErrors = true;
			printf("ERROR: Failed to read %s\n",optVal.getString());
			}
		    else
			{
			const char *charset = "us-ascii";

			if (sbInCharset.getNumChars())
			    {
			    charset = sbInCharset.getString();
			    }

			if (!email.SetMbPlainTextBody(charset,data))
			    {
			    hasErrors = true;
			    printf("ERROR: Failed to set plain text body.\n");
			    }
			}
		    i++;
		    }
		else if (strcmp(opt,"-log") == 0)
		    {
		    sbXmlLog.clear();
		    sbXmlLog.append(optVal.getString());
		    i++;
		    }




		}
	    }
	}






    if (!unlocked)
	{
	printf("ERROR: Component locked\n");
	exit(1);
	}













    if (sbZipFilename.getNumChars())
	{
	if (!email.ZipAttachments(sbZipFilename.getString()))
	    {
	    hasErrors = true;
	    printf("ERROR: Failed to zip attachments.\n");
	    }
	}

    if (hasErrors) exit(1);

    if (!hasSmtpServer)
	{
	printf("ERROR: Missing required option (-s) for the SMTP hostname\n");
	exit(1);
	}

    if (sbSendCharset.getNumChars())
	{
	email.put_Charset(sbSendCharset.getString());
	}

    if (sbAuthMethod.getNumChars())
	{
	mailman.put_SmtpAuthMethod(sbAuthMethod.getString());
	}

    bool b = mailman.SendEmail(email);
    if (b)
	{
	printf("Mail Sent");
	if (sbXmlLog.getNumChars())
	    {
	    mailman.SaveLastError(sbXmlLog.getString());
	    }
	}
    else
	{
	if (sbXmlLog.getNumChars())
	    {
	    mailman.SaveLastError(sbXmlLog.getString());
	    }
	printf("ERROR: Failed to send mail.\n");
	exit(1);
	}

    return 0;
}

