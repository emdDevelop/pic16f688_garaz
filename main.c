#include <glib.h>
#include <gtk/gtk.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/*
1. Create a socket
2. Connect to remote server
3. Send some data
4. Receive a reply

Its useful to know that your web browser also does the same thing when you open www.google.com
This kind of socket activity represents a CLIENT. A client is a system that connects to a
remote system to fetch or retrieve data.
*/

GtkWidget *statusBar;
GtkWidget *window;
GtkWidget *label_DoorStatus;
GtkWidget *textView;
GtkWidget *comboBox;

GtkTextBuffer *buffer;
GtkTextMark *mark;
GtkTextIter iter_start;
GtkTextIter iter_end;
gchar *Ip;

WSADATA wsa;
SOCKET s;

static void status (void)
{
    char *message;
    message = "STATUS";//message to send

    if( send(s , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return ;
    }
    puts("Data Send\n");
}

static void setUp (GtkWidget *widget)
{
  //Send some data
    char *message;
    message = "UP";//message to send

    if( send(s , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return ;
    }
    puts("Data Send\n");
}

static void setDown (GtkWidget *widget,gpointer window)
{
  //Send some data
    char *message;
    message = "DOWN";//message to send

    if( send(s , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return ;
    }
    puts("Data Send\n");
}

static void setStop (GtkWidget *widget,gpointer window)
{
  //Send some data
    char *message;
    message = "TERMINATE";//message to send

    if( send(s , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return ;
    }
    puts("Data Send\n");
}

static void picConnect (GtkWidget *widget){
    u_long iMode=1;
    struct sockaddr_in server;
    gchar *checkEntry;

      gtk_text_buffer_get_start_iter (buffer, &iter_start);
      gtk_text_buffer_get_end_iter (buffer, &iter_end);
      gtk_text_buffer_delete(buffer,&iter_start,&iter_end );

    checkEntry=gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(comboBox));//get active combo text

   if(strcmp(checkEntry,"") == 0)
   {
   gtk_text_buffer_insert (buffer,&iter_start,"Please select IP\n",-1);
   g_free(checkEntry);
   }else{

    printf("\nInitialising Winsock...");
    gtk_text_buffer_insert (buffer,&iter_start,"Initialising Winsock...\n",-1);

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
       return ;
    }

    printf("\nInitialised.");
    gtk_text_buffer_insert (buffer,&iter_start,"Initialised\n",-1);

      if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("\nCould not create socket : %d" , WSAGetLastError());
    }

    printf("\nSocket created.\n");
    gtk_text_buffer_insert (buffer,&iter_start,"Socket created.\n",-1);

    server.sin_addr.s_addr = inet_addr(Ip); //pic ip address
    server.sin_family = AF_INET; //The Internet Protocol version 4 (IPv4) address family.
    //server.sin_port = htons( 8001 ); //pic port nikos
    server.sin_port = htons( 80 ); //spiti anna

    gtk_text_buffer_insert (buffer,&iter_start,"Waiting to connect.....\n",-1);

    while(gtk_events_pending())//thread enable
    gtk_main_iteration();

  //Connect to remote server
    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        closesocket(s);
        WSACleanup();
        puts("connect error");
        gtk_text_buffer_insert (buffer,&iter_start,"Connection error\n",-1);
        return ;
    }

    ioctlsocket(s, FIONBIO, &iMode);

    puts("Connected");
    gtk_text_buffer_insert (buffer,&iter_start,"Connected\n",-1);

    //get status
     status();

     char server_reply[1024];
     int recv_size;

     for(;;)
{
while(gtk_events_pending())//thread enable
             gtk_main_iteration();
       //Receive a reply from the server
  do{
        memset(server_reply,0,1024);//clear the variable
      if((recv_size = recv(s , server_reply , 1024 , 0))<0)
      {
        break;
      }else{
       server_reply[recv_size] = '\0';
       gtk_label_set_text (GTK_LABEL(label_DoorStatus),server_reply);
       printf("\n%s",server_reply);
      }

   }while(recv_size>0);
}

   }//end of else
}


static void cb_changed(GtkComboBoxText *combo){

    /* Obtain currently selected string from combo box */
     Ip = gtk_combo_box_text_get_active_text( combo );
    /* Free string */

}

 static void help_message () {

        GtkWidget *dialog, *label,*box;

        /* Create the widgets */

        dialog = gtk_dialog_new_with_buttons ("Message",
                                              NULL,
                                              GTK_DIALOG_DESTROY_WITH_PARENT,
                                              GTK_STOCK_OK,
                                              GTK_RESPONSE_NONE,
                                              NULL);
         box=gtk_dialog_get_content_area (GTK_DIALOG (dialog));
        label = gtk_label_new ("This apliccation is a Digital thermometer.Its working with a pic\nconnected in serial communication");


        /* Ensure that the dialog box is destroyed when the user responds. */

        g_signal_connect_swapped (dialog,"response",G_CALLBACK (gtk_widget_destroy),dialog);

        /* Add the label, and show everything we've added to the dialog. */

        gtk_container_add (GTK_CONTAINER (box), label);

        gtk_widget_show_all (dialog);
     }

static void close(){

    closesocket(s);
    WSACleanup();
    g_free(Ip );
    //gtk_main_quit ();
    exit(0);
}

static void closeConnection(){
    g_free(Ip );
    closesocket(s);
    WSACleanup();

      gtk_text_buffer_get_start_iter (buffer, &iter_start);
      gtk_text_buffer_get_end_iter (buffer, &iter_end);
      gtk_text_buffer_delete(buffer,&iter_start,&iter_end );
      gtk_text_buffer_insert (buffer,&iter_start,"Connection closed\n",-1);

}

static void activate (GtkApplication *app,gpointer user_data)
{
  //buttons.....................................
  GtkWidget *btn_up;
  GtkWidget *btn_down;
  GtkWidget *btn_stop;
  GtkWidget *btn_connect;
  GtkWidget *btn_closeConnection;
  GtkWidget *btn_help;
  GtkWidget *btn_exit;
  //.................................end buttons

  GtkWidget *label_Title;
  GtkWidget *label_DoorText;
  GtkWidget *label_UpDoorText;
  GtkWidget *label_DownDoorText;
  GtkWidget *label_StopDoorText;
  GtkWidget *label_SetComPortText;
  GtkWidget *scrollWidget;

  //layout.......................................
  GtkWidget *box;
  GtkWidget *Hbox;
    GtkWidget *H_Hbox;
    GtkWidget *H_Bbox;
  GtkWidget *Mbox;
    GtkWidget *M_Hbox;
    GtkWidget *M_Mbox;
    GtkWidget *M1_Mbox;
    GtkWidget *M_Bbox;
  GtkWidget *textbox;
    GtkWidget *text_Hbox;
    GtkWidget *text_Bbox;
  GtkWidget *Bbox;
    GtkWidget *B_Hbox;
    GtkWidget *B_Bbox;
  //..................................end layout

//set buttons,labels,etc.................................................................
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Garaz Control");
  label_Title=gtk_label_new("  Garaz Control Application");
  label_DoorText=gtk_label_new("  Garaz Door Status:");
  label_DoorStatus=gtk_label_new("....");
  label_UpDoorText=gtk_label_new("  Press UP to move Door up");
  btn_up = gtk_button_new_with_label ("UP");
  label_DownDoorText=gtk_label_new("  Press DOWN to move Door down");
  btn_down = gtk_button_new_with_label ("DOWN");
  label_StopDoorText=gtk_label_new("  Press STOP to stop moving Door ");
  btn_stop = gtk_button_new_with_label ("STOP");
  label_SetComPortText=gtk_label_new("  Set Pic IP");
  comboBox=gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.5");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.6");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.7");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.8");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.9");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.10");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.11");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.12");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.13");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.14");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.15");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.16");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.110");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.111");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.112");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.113");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.114");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(comboBox),NULL,"192.168.1.115");

  textView=gtk_text_view_new();
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
  gtk_text_buffer_get_start_iter(buffer, &iter_start);
  gtk_text_buffer_get_end_iter(buffer, &iter_end);
  mark = gtk_text_buffer_get_insert(buffer);
  gtk_text_buffer_get_iter_at_mark(buffer, &iter_start, mark);
  scrollWidget=gtk_scrolled_window_new (NULL,NULL);

  btn_connect=gtk_button_new_with_label ("Connect to Pic");
  btn_closeConnection=gtk_button_new_with_label ("Close Connection");
  btn_help=gtk_button_new_with_label ("Help");
  btn_exit=gtk_button_new_with_label ("Exit");
  statusBar = gtk_statusbar_new();
  //context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR (statusBar), "serial about");
//..........................................................................end of settings

  //Layout setting........................................................................
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL,30);
  Hbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,40);
    H_Hbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
    H_Bbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,30);
  Mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,30);
    M_Hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    M_Mbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    M1_Mbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    M_Bbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
  textbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,30);
    text_Hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    text_Bbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
  Bbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,30);
    B_Hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    B_Bbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);

  gtk_box_pack_start (GTK_BOX(box),Hbox,0,1,0);
    gtk_box_pack_start (GTK_BOX(Hbox),H_Hbox,0,1,0);
    gtk_box_pack_end (GTK_BOX(Hbox),H_Bbox,0,1,0);
  gtk_box_pack_start (GTK_BOX(box),Mbox,0,1,0);
    gtk_box_pack_start (GTK_BOX(Mbox),M_Hbox,0,1,0);
    gtk_box_pack_start (GTK_BOX(Mbox),M_Mbox,0,1,0);
    gtk_box_pack_start (GTK_BOX(Mbox),M1_Mbox,0,1,0);
    gtk_box_pack_end (GTK_BOX(Mbox),M_Bbox,0,1,0);
  gtk_box_pack_start (GTK_BOX(box),textbox,0,1,0);
    gtk_box_pack_start (GTK_BOX(textbox),text_Hbox,0,1,0);
    gtk_box_pack_end (GTK_BOX(Mbox),text_Bbox,0,1,0);
  gtk_box_pack_end (GTK_BOX(box),Bbox,0,1,0);
    gtk_box_pack_start (GTK_BOX(Bbox),B_Hbox,0,1,0);
    gtk_box_pack_end (GTK_BOX(Bbox),B_Bbox,0,1,0);

  gtk_container_add (GTK_CONTAINER (window),box);

  gtk_container_add (GTK_CONTAINER (H_Hbox), label_Title);
  gtk_container_add (GTK_CONTAINER (H_Bbox), label_DoorText);
  gtk_container_add (GTK_CONTAINER (H_Bbox), label_DoorStatus);

  gtk_container_add (GTK_CONTAINER (M_Hbox), label_UpDoorText);
  gtk_container_add (GTK_CONTAINER (M_Hbox), btn_up);
  gtk_container_add (GTK_CONTAINER (M_Mbox), label_DownDoorText);
  gtk_container_add (GTK_CONTAINER (M_Mbox), btn_down);
  gtk_container_add (GTK_CONTAINER (M1_Mbox), label_StopDoorText);
  gtk_container_add (GTK_CONTAINER (M1_Mbox), btn_stop);
  gtk_container_add (GTK_CONTAINER (M_Bbox), label_SetComPortText);
  gtk_container_add (GTK_CONTAINER (M_Bbox), comboBox);

  gtk_container_add (GTK_CONTAINER (text_Hbox), scrollWidget);
  gtk_container_add (GTK_CONTAINER (scrollWidget), textView);


  gtk_container_add (GTK_CONTAINER (B_Hbox), btn_connect);
  gtk_container_add (GTK_CONTAINER (B_Hbox), btn_closeConnection);
  gtk_container_add (GTK_CONTAINER (B_Hbox), btn_help);
  gtk_container_add (GTK_CONTAINER (B_Hbox), btn_exit);
  gtk_container_add (GTK_CONTAINER (B_Bbox), statusBar);
  //...........................................................................end of layout setting


  //Styling.......................................................................................
  gtk_widget_set_size_request(window, 500, 400);
  gtk_widget_set_size_request(scrollWidget, 300, 100);

  GdkColor color;
  gdk_color_parse( "red", &color );

  gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
  gtk_widget_override_font (label_Title,pango_font_description_from_string ("Arial 20"));
  gtk_widget_override_font (label_DoorText,pango_font_description_from_string ("Arial 16"));
  gtk_widget_override_font (label_DoorStatus,pango_font_description_from_string ("Arial 16"));
  gtk_widget_modify_fg( GTK_WIDGET(label_DoorStatus), GTK_STATE_NORMAL,&color );
  gtk_widget_override_font (label_UpDoorText,pango_font_description_from_string ("Arial 16"));
  gtk_widget_override_font (label_DownDoorText,pango_font_description_from_string ("Arial 16"));
  gtk_widget_override_font (label_StopDoorText,pango_font_description_from_string ("Arial 16"));
  gtk_widget_override_font (label_SetComPortText,pango_font_description_from_string ("Arial 16"));
  gtk_widget_set_margin_right (label_UpDoorText,10);
  gtk_widget_set_margin_right (btn_stop,10);
  gtk_widget_set_margin_left (btn_connect,150);
  gtk_widget_set_margin_left (scrollWidget,80);

  //...............................................................................end of styling

  //Events........................................................................................
  g_signal_connect (btn_up, "clicked", G_CALLBACK (setUp), NULL);
  g_signal_connect (btn_down, "clicked", G_CALLBACK (setDown), NULL);
  g_signal_connect (btn_stop, "clicked", G_CALLBACK (setStop), NULL);
  g_signal_connect (btn_connect, "clicked", G_CALLBACK (picConnect), NULL);
  g_signal_connect (comboBox, "changed",G_CALLBACK( cb_changed ), NULL );
  g_signal_connect (btn_closeConnection, "clicked", G_CALLBACK (closeConnection),NULL);
  g_signal_connect (btn_help, "clicked", G_CALLBACK (help_message), NULL);
  g_signal_connect (btn_exit, "clicked", G_CALLBACK (close), NULL);

//.................................................................................end of events


  gtk_widget_show_all (window);
}

int main(int argc,char **argv)
{

  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

   return status;
}
