/* DDE client for XEmacs.
   Copyright (C) 2002 Alastair J. Houghton

   This file is part of XEmacs.

   XEmacs is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   XEmacs is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with XEmacs; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */  
    
/* Synched up with: Not in FSF. */ 
    
/* -- Pre-Include Defines --------------------------------------------------- */ 
    
#define STRICT
    
/* -- Includes -------------------------------------------------------------- */ 
    
#include <windows.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif	/* 
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>





/* -- Post-Include Defines -------------------------------------------------- */ 
    
/* Timeouts & delays */ 
#define CONNECT_DELAY		500		/* ms */
#define TRANSACTION_TIMEOUT	5000		/* ms */
#define MAX_INPUT_IDLE_WAIT     INFINITE	/* ms */
    
/* DDE Strings */ 
#define SERVICE_NAME	"XEmacs"
#define TOPIC_NAME	"System"
#define COMMAND_FORMAT	"[open(\"%s%s\")]"
    
/* XEmacs program name */ 
#define PROGRAM_TO_RUN	"xemacs.exe"
    
/* -- Constants ------------------------------------------------------------- */ 
    
/* -- Global Variables ------------------------------------------------------ */ 
    


/* -- Function Declarations ------------------------------------------------- */ 
    
				   
				   

		     





/* -- Function Definitions -------------------------------------------------- */ 
    
/*
 * Name    : ddeCallback
 * Function: Gets called by DDEML.
 *
 */ 
    
ddeCallback(UINT uType, UINT uFmt, HCONV hconv, 
	    HDDEDATA hdata, 
{
	



/*
 * Name    : WinMain
 * Function: The program's entry point function.
 *
 */ 

WinMain(HINSTANCE hInst, 
{
	
	
	
	
	    /* Initialise the DDEML library */ 
	    uiRet = DdeInitialize(&idInst, 
				  (PFNCALLBACK) ddeCallback,
				  
				  
	
		
		
			    "Could not initialise DDE management library.",
			    
		
		
	
	    /* Open a conversation */ 
	    hConv = openConversation();
	
		
		
		    /* OK. Next, we need to parse the command line. */ 
		    ret = parseCommandLine(hConv, lpCmdLine);
		
		    /* Close the conversation */ 
		    closeConversation(hConv);
		
	
	



/*
 * Name    : openConversation
 * Function: Start a conversation.
 *
 */ 

{
	
	
	
	    /* Get the application (service) name */ 
	    hszService =
	    DdeCreateStringHandle(idInst, 
	
		
		
			    
		
		
	
	    /* Get the topic name */ 
	    hszTopic = DdeCreateStringHandle(idInst, 
	
		
		
			    
		
		
	
	    /* Try to connect */ 
	    hConv = DdeConnect(idInst, hszService, hszTopic, NULL);
	
		
		
		
		
		
		    /* Try to start the program */ 
		    ZeroMemory(&sti, sizeof(sti));
		
		
		     (NULL, PROGRAM_TO_RUN, NULL, NULL, FALSE, 0, 
		      &sti, &pi))
			
			
				    
			
			
		
		    /* Wait for the process to enter an idle state */ 
		    WaitForInputIdle(pi.hProcess, MAX_INPUT_IDLE_WAIT);
		
		    /* Close the handles */ 
		    CloseHandle(pi.hThread);
		
		
		    /* Try to connect */ 
		    for (n = 0; n < 5; n++)
			
			
			
			    DdeConnect(idInst, hszService, hszTopic, NULL);
			
				
			
		
			
			
			    /* Still couldn't connect. */ 
			    MessageBox(NULL, "Could not connect to DDE server.",
				       
				       MB_ICONEXCLAMATION | MB_OK);
			
			
		
	
	    /* Release the string handles */ 
	    DdeFreeStringHandle(idInst, hszService);
	
	
      
		
	
		
	
		
	



/*
 * Name    : closeConversation
 * Function: Close a conversation.
 *
 */ 

{
	
	    /* Shut down */ 
	    DdeDisconnect(hConv);


/*
 * Name    : doFile
 * Function: Process a file.
 *
 */ 

{
	
	
	
	    /* Calculate the buffer length */ 
	    len = strlen(lpszFileName1) + strlen(lpszFileName2) 
	    +strlen(COMMAND_FORMAT);
	
	    /* Allocate a buffer */ 
	    buf = (char *)xmalloc(len);
	
		
		
			    MB_ICONEXCLAMATION | MB_OK);
		
		
	
	    /* Build the command */ 
	    len = wsprintf(buf, COMMAND_FORMAT, lpszFileName1, lpszFileName2);
	
	
	    /* OK. We're connected. Send the message. */ 
	    DdeClientTransaction(buf, len, hConv, NULL, 
				 TRANSACTION_TIMEOUT, NULL);
	
	



/*
 * Name    : getNextArg
 * Function: Retrieve the next command line argument.
 *
 */ 

{
	
	
	
	
	
	    /* Skip whitespace */ 
	    while (*p && isspace(*p))
		
	
	    /* If this is the end, return NULL */ 
	    if (!*p)
		
	
	    /* Remember where we are */ 
	    start = p;
	
	    /* Find the next whitespace character outside quotes */ 
	    if (*p == '"')
		
	
		
		
			
		
			
			
			
		
			
				
			
			
				
			
			
		
			
				
			
			else if (!in_quotes)
				
			
				
			
		
	
	    /* Work out the length */ 
	    length = p - start;
	
	    /* Strip quotes if the argument is completely quoted */ 
	    if (all_in_quotes)
		
		
		
		
	
	    /* Copy */ 
	    buf = (char *)xmalloc(length + 1);
	
		
	
	
	
	    /* Return the pointer and length */ 
	    *ptr = p;
	
	



/*
 * Name    : parseCommandLine
 * Function: Process the command line. This program accepts a list of strings
 *         : (which may contain wildcards) representing filenames.
 *
 */ 

{
	
	
	
	
	
	
	
	    /* Retrieve arguments */ 
	    while ((arg =
		    getNextArg((const char **)&lpszCommandLine, &len)) != NULL)
		
		
		    /* First find the canonical path name */ 
		    fullpath = filepart = NULL;
		
		
		
			
			
				    
			
			
			
			
		
		
		    /* Find the first matching file */ 
		    hFindFile = FindFirstFile(arg, &wfd);
		
			
		
		else
			
			
			    /* Chop off the file part from the full path name */ 
			    if (filepart)
				
			
			    /* For each matching file */ 
			    do
				
				
				    /* Process it */ 
				    ret =
				    doFile(hConv, fullpath, wfd.cFileName);
				
					
			
			
			
		
		    /* Release the path name buffers */ 
		    free(fullpath);
		
		
			
		
	


{
	
	


/* Print error message.  `s1' is printf control string, `s2' is arg for it. */ 
static void 
{
	
	
	


/* Like malloc but get fatal error if memory is exhausted.  */ 

{
	
	
		
	


