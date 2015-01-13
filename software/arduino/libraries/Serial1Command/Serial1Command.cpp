/**
 * Serial1Command - A Wiring/Arduino library to tokenize and parse commands
 * received over a Serial1 port.
 * 
 * Copyright (C) 2012 Stefan Rado
 * Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
 *                    http://husks.wordpress.com
 * 
 * Version 20120522
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Serial1Command.h"

/**
 * Constructor makes sure some things are set.
 */
Serial1Command::Serial1Command()
  : commandList(NULL),
    commandCount(0),
    defaultHandler(NULL),
    term('\n'),           // default terminator for commands, newline character
    last(NULL)
{
  strcpy(delim, " "); // strtok_r needs a null-terminated string
  clearBuffer();
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void Serial1Command::addCommand(const char *command, void (*function)()) {
  #ifdef Serial1COMMAND_DEBUG
    Serial1.print("Adding command (");
    Serial1.print(commandCount);
    Serial1.print("): ");
    Serial1.println(command);
  #endif

  commandList = (Serial1CommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(Serial1CommandCallback));
  strncpy(commandList[commandCount].command, command, Serial1COMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function = function;
  commandCount++;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void Serial1Command::setDefaultHandler(void (*function)(const char *)) {
  defaultHandler = function;
}


/**
 * This checks the Serial1 stream for characters, and assembles them into a buffer.
 * When the terminator character (default '\n') is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void Serial1Command::readSerial() {
  while (Serial1.available() > 0) {
    char inChar = Serial1.read();   // Read single available character, there may be more waiting
    #ifdef Serial1COMMAND_DEBUG
      Serial1.print(inChar);   // Echo back to Serial1 stream
    #endif

    if (inChar == term) {     // Check for the terminator (default '\r') meaning end of command
      #ifdef Serial1COMMAND_DEBUG
        Serial1.print("Received: ");
        Serial1.println(buffer);
      #endif

      char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
      if (command != NULL) {
        boolean matched = false;
        for (int i = 0; i < commandCount; i++) {
          #ifdef Serial1COMMAND_DEBUG
            Serial1.print("Comparing [");
            Serial1.print(command);
            Serial1.print("] to [");
            Serial1.print(commandList[i].command);
            Serial1.println("]");
          #endif

          // Compare the found command against the list of known commands for a match
          if (strncmp(command, commandList[i].command, Serial1COMMAND_MAXCOMMANDLENGTH) == 0) {
            #ifdef Serial1COMMAND_DEBUG
              Serial1.print("Matched Command: ");
              Serial1.println(command);
            #endif

            // Execute the stored handler function for the command
            (*commandList[i].function)();
            matched = true;
            break;
          }
        }
        if (!matched && (defaultHandler != NULL)) {
          (*defaultHandler)(command);
        }
      }
      clearBuffer();
    }
    else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (bufPos < Serial1COMMAND_BUFFER) {
        buffer[bufPos++] = inChar;  // Put character into buffer
        buffer[bufPos] = '\0';      // Null terminate
      } else {
        #ifdef Serial1COMMAND_DEBUG
          Serial1.println("Line buffer is full - increase Serial1COMMAND_BUFFER");
        #endif
      }
    }
  }
}

/*
 * Clear the input buffer.
 */
void Serial1Command::clearBuffer() {
  buffer[0] = '\0';
  bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *Serial1Command::next() {
  return strtok_r(NULL, delim, &last);
}
