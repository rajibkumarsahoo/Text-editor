# Text-editor

Firstly, we looked for different text editors available. We found nearly forty-eight of them.

Secondly,we tried to go through the source code of some popular text-editors like Sublime text and Notepad++.
Then, we visited the github reopositories of these text-editors and found out Notepad++ which is an open source text-editor has been developed with Microsoft Visual Studio and with C/C++ compilers. 

Also, there are both paid and free versions available for the text editors. The source-code for free text-editors are available online but there is no data on the paid versions of text editors.
We did brainstorming as a team and worked out our way ahead.

# Customer Journey

We analyzed the customer journey to come up with User requirements. We scoped the problem as per the time constraint and team size constraints.
We identified the following major requirements:

# Icon: 
Open UI of editor on double click or select and press enter key

# User Interface (UI):
Text Editing Area: The area where we can input , edit and delete the text
Menu Bar: For various functionalities and options
Status Bar: Display information about the status of the document, such as character count, line count and cursor position
File Navigation: Option to create new file, open existing file, save current file
Insert: pictures or related data

# Text Editing Features:
Basic Text Editing: Allows users to add, edit and delete text
Copy, cut and paste Functionality
Find and Replace Option
Font Style: We will be providing functionality for 5 fonts
Text Alignment: Options for left, right and center alignment
Font Formatting: Size, Italics, Underline
Bullets: 2 types of Bullets
Font Colour: Only Black colour functionality 

# File Management:
Open and Save:The functionality to open a new file/ existing file and save a file in .txt format

# Shortcuts:
Ctrl+S: To save the file
Ctrl+C: To Copy the text
Ctrl+V: To Print the text
Ctrl+F: Search a string
Ctrl+R: Search and replace a string
Ctrl+Q: Quit the editor

# Timeline and Estimation of Time:
 UI : By 10 Oct 23
 Editing Features : 05 Nov 23
 Completion : 15 Nov 23
# Overview
At the very outset we need to first read the keypresses from the user. In order to do this we include unistd.h header file and then utilise the read() system function. The read() will receive input from user through standard input devices like keyboard @1 byte each time during keypress. A while loop will keep taking inputs from the user and pass it to a variable until there are no more bytes to read. The EOF will be understood  by read() when there is no input or we give CTRL+D command. CTRL+C command terminates the program immediately.
Instead of having CTRL+C to quit the program we try and put q+ENTER to signal end of file to read(). In order to achieve this we put AND condition in while loop so that the read() understands EOF when the variable encounters q and then we press ENTER to come out of the program.
Next in order to convert our Nano text editor into Raw mode we need to disable the Display/ECHO feature of the terminal. We will be using tcgetattr() system function to store the attributes of the terminal before it is subjected to raw mode and tcgetattr() system function to push those attributes back to terminal when we quit Raw mode of the terminal. termios.h header file contains the tcgetattr(),tcsetattr(),struct termios,ECHO and TCSAFLUSH.
Now atexit() which is a part of the stdlib.h header file will be put into the function that enables Raw mode which will accept the function to disable Raw mode as input so that we may secure the original attributes of the terminal before entering Raw mode. We store the actual attributes in the global variable called t_o. TCSAFLUSH is passed to tcsetattr() to ensure that there is no leftover input fed into the shell when the program quits.
Next in order to turn off the default mode of the terminal we turn off the ICANON flag. This will also help us to read inputs byte by byte instead of line by line. We do this in the raw_enable().Now in order to display the keypresses we need two more header files viz ctype.h and stdio.h for iscntrl() and printf() system functions respectively. Eventually we disable multiple CTRL keys through the raw_enable function.
Now we turn off all output processing by disabling \r\n and \n which are ON by default. This is done by disabling the OPOST flag in raw_enable function. Also in order to timeout the read() when there is no input we will be using the VTIME[] and VMIN[] to return read() when it reads an input of 1 byte. We set VTIME[] as one tenth of a second.
Now we introduce error handling in our program by writing a function die(). This function uses perror from stdio.h header file and exit() from stdlib.h.The die() function will print an error message and exit the program when it encounters any error. perror() however looks at the errno.h header file for printing a descriptive message as output on finding error.
Next we enable CTRL+Q to quit the program instead of pressing ENTER after q. This is achieved by doing an AND 
between q and the CTRL.
Now we will try and simplify the main() with the rk_editor() and k_p_e() functions for low-level keypress reading and mapping keypress to editor operations respectively.
Lastly we clear the screen to provide editor's user interface after each keypress.This is achieved by r_e_s() function. ANSI escape code sequence instrusts the terminal to clear its screen.

Post putting the terminal into raw mode we create a datatype for storing a row of text in our editor, store multiple lines, enable vertical and horizontal scrolling, improve tabs and cursor operations, page up and down operations and finally establishing a status bar before moving onto text-editing features. Then we enable text editing features like inserting ordinary characters, preventing insertion of special characters, enabling backspacing and ENTER operations. Then we enabled functions like quit confirmation, string search, search and replace string operations.  
