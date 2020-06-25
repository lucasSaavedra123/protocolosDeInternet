import copy
import sys
import os

def isNumberBetween(number,firstValue,secondValue):
    isBetween = False

    if number >= firstValue and number <= secondValue:
        isBetween = True

    return isBetween

def isALetter(letter):
    isLetter = False

    if len(letter) == 1 and (isNumberBetween(ord(letter),65,90) or isNumberBetween(ord(letter),97,122)):
        isLetter = True

    return isLetter

def getPositionOfLetter(letter):
    position = 0

    if not isALetter(letter):
        raise NameError("It's not a letter")


    letter = letter.upper()

    position = ord(letter) - 65

    return position

def getLetterByPosition(position):
    letter = 0

    if not isNumberBetween(position,0,25):
        raise NameError("It's not a valid position")

    letter = chr(position + 97)

    return letter

def listToString(listOfCharacters):
    string = ""

    for character in listOfCharacters:
        string += character

    return string

def encrypt(key, string):
    listOfEncryptedLetters = []

    for character in string:

        if isALetter(character):
            actualPosition = getPositionOfLetter(character)
            newPosition = ((actualPosition + key) % 26)

            if character.isupper():
                listOfEncryptedLetters.append(getLetterByPosition(newPosition).upper())
            elif character.islower():
                listOfEncryptedLetters.append(getLetterByPosition(newPosition).lower())
        else:
            listOfEncryptedLetters.append(character)

    encryptedString = listToString(listOfEncryptedLetters)

    return encryptedString

def decrypt(key, string):
    listOfDecryptedLetters = []

    for character in string:

        if isALetter(character):
            actualPosition = getPositionOfLetter(character)
            newPosition = ((actualPosition - key) % 26)

            if character.isupper():
                listOfDecryptedLetters.append(getLetterByPosition(newPosition).upper())
            elif character.islower():
                listOfDecryptedLetters.append(getLetterByPosition(newPosition).lower())
        else:
            listOfDecryptedLetters.append(character)

    decryptedString = listToString(listOfDecryptedLetters)

    return decryptedString

def getLinesOfFile(fileName):
    fileToEncrypt = open(fileName, "r")
    arrayOfLines = []

    for line in fileToEncrypt:
        arrayOfLines.append(line)

    fileToEncrypt.close()

    return arrayOfLines

def encryptLines(key,arrayOfLines):
    linesEncrypted = []

    for line in arrayOfLines:
        linesEncrypted.append(encrypt(key,line))

    return linesEncrypted

def decryptLines(key,arrayOfLines):
    linesDecrypted = []

    for line in arrayOfLines:
        linesDecrypted.append(decrypt(key,line))

    return linesDecrypted

def saveLinesInFile(lines, fileName):
    fileToSave = open(fileName, "w")

    for line in lines:
        fileToSave.write(line)

    fileToSave.close()



def main():

    quantityOfArguments = len(sys.argv)
    argumentsList = sys.argv

    if quantityOfArguments != 4:
        raise NameError("usage: file name to encrypt, e (encrypt) or d (decrypt), and the key")        

    fileName = argumentsList[1]
    command = argumentsList[2]
    key = int(argumentsList[3])

    arrayOfLines = getLinesOfFile(fileName)

    if command == 'e':
        arrayOfLinesToSave = encryptLines(key,arrayOfLines)
    elif command == 'd':
        arrayOfLinesToSave = decryptLines(key,arrayOfLines)
    else:
        raise NameError("Invalid Command")       
    
    saveLinesInFile(arrayOfLinesToSave,command+"_"+fileName)


    return 0

main()