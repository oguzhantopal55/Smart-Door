#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

const char* DIR = "./Visitors/";
const char* prefix = "image";

const char* sender = "sender@gmail.com";
const char* password = "12345678";
const char* receiver = "receiver@gmail.com";

void send_mail(const char* filename) {
    char command[500];
    sprintf(command, "echo \"Picture is Attached.\" | mail -s \"New Visitor\" -A %s -r %s %s", filename, sender, receiver);
    system(command);
}

void capture_img() {
    printf("Capturing\n");
    if (system("test -d ./Visitors/") != 0) {
        system("mkdir ./Visitors/");
    }
    
    char command[200];
    sprintf(command, "ls -1t %s | grep \"%s[0-9][0-9][0-9].jpg\" | head -n 1", DIR, prefix);
    
    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("Error executing command.\n");
        return;
    }
    
    char output[100];
    fgets(output, sizeof(output), pipe);
    pclose(pipe);
    
    int count = 0;
    if (output[0] != '\0') {
        sscanf(output, "%*[^0-9]%d", &count);
        count++;
    }
    
    char filename[100];
    sprintf(filename, "%s%s%03d.jpg", DIR, prefix, count);
    
    sprintf(command, "capture -o %s", filename);
    system(command);
    
    send_mail(filename);
}

int main(void) {
    wiringPiSetup();
    pinMode(15, INPUT);
    pinMode(11, INPUT);
    
    int i = 0;
    
    while (1) {
        int motion = digitalRead(11);
        int button = digitalRead(15);
        
        if (motion == HIGH) {
            printf("Motion Detected\n");
            capture_img();
        }
        
        if (button == HIGH) {
            printf("Button Pressed\n");
            capture_img();
        }
        
        delay(100);
    }
    
    return 0;
}