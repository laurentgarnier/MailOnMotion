#include <ESP_Mail_Client.h>
#include "camera_management.h"

String emailSenderAccount = "maisoncroziergarnier@gmail.com";
String emailSenderPassword = "rqwtivssjxikkvsb";
String smtpServer = "smtp.gmail.com";
int smtpServerPort = 465;
String emailSubject = "J ai vu Filou";
String emailRecipient = "lologar69@gmail.com";
String emailsubject = "Filou cam";

SMTPSession smtp;

bool sendMail(acquisitionResult acquisitions[], int imagesNumber)
{   
    Serial.println("Envoi email...");
    ESP_Mail_Session session;
    smtp.debug(1);
   
    session.server.host_name = smtpServer;
    session.server.port = smtpServerPort;
    session.login.email = emailSenderAccount;
    session.login.password = emailSenderPassword;

    SMTP_Message message;

    message.enable.chunking = true;
    message.sender.name = emailsubject;
    message.sender.email = emailSenderAccount;
    message.subject = emailSubject;
    message.addRecipient("lolo", emailRecipient);

    String content = "Free heap: " + String(ESP.getFreeHeap());
    content += "\n\rFree PSRAM: " + String(ESP.getFreePsram());

    message.text.content = content;
    message.text.charSet = "utf-8";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_base64;

    message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;

    message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
    
    for (int indexBuffer = 0; indexBuffer < imagesNumber; indexBuffer++)
    {
        String nomImage = String("photo_" + String(indexBuffer+1) + ".jpg");
        Serial.println("Ajout image " + nomImage);
        SMTP_Attachment attachement;
        attachement.descr.filename = nomImage.c_str();
        attachement.descr.mime = "image/jpg";
        attachement.blob.data = acquisitions[indexBuffer].buffer;
        attachement.blob.size = acquisitions[indexBuffer].bufferLength;
        attachement.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
        message.addAttachment(attachement);
    }

    
    if (!smtp.connect(&session))
    {
        Serial.println("Erreur de connection, " + smtp.errorReason());
        return false;
    }

    if (!MailClient.sendMail(&smtp, &message, true))
    {
        Serial.println("Erreur envoi mail, " + smtp.errorReason());
        smtp.closeSession();
        return false;
    }
    
    return true;
}

