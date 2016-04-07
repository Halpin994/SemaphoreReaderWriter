// Semaphore ReaderWriter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <list>
#include <SDL.h>
#include <fstream>
#include <string>
using namespace std;

int nr = 0; //num of readers

//Data access semaphore
SDL_sem* gDataLock = NULL;
SDL_sem* mutexR = NULL;

//bool threadAlive = true;

int ReaderThread(void * data);
int WriterThread(void * data);

int _tmain(int argc, _TCHAR* argv[])
{
	list<SDL_Thread*> readerThreads;
	list<SDL_Thread*> writerThreads;

	//Initialize semaphore
	gDataLock = SDL_CreateSemaphore(1);
	mutexR = SDL_CreateSemaphore(1);

	for (int i = 0; i < 15; i++)
	{
		writerThreads.push_back(SDL_CreateThread(WriterThread, "writer", &i));
	}
	for (int i = 0; i < 10; i++)
	{
		readerThreads.push_back(SDL_CreateThread(ReaderThread, "reader", NULL));
	}
	//threadAlive = false;
	system("PAUSE");
	return 0;
}

int ReaderThread(void * data)
{
	while (true)
	{

		SDL_SemWait(mutexR);
		nr++; //number of readers
		//if first, get lock to allow you to read from db
		if (nr == 1)
		{
			SDL_SemWait(gDataLock);
		}
		SDL_SemPost(mutexR);
		//read the database;
		cout << "reading from text file\n";

		ifstream myfile("text.txt");
		string line;
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				cout << line << '\n';
			}
			myfile.close();
		}

		SDL_SemWait(mutexR);
		nr--;
		if (nr == 0) //if last reader, release the lock
		{
			SDL_SemPost(gDataLock);
		}
		SDL_SemPost(mutexR);

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 640);
	}
	return 0;
}
int WriterThread(void * data)
{
	int i = *(int *)data;
	while (true)
	{
		SDL_SemWait(gDataLock);
		//write the database;
		cout << "writing to text file\n";

		ofstream myfile;
		myfile.open("text.txt" , ios::app); //allows to write multiple lines 
		myfile << "Last writer thread: " << i << endl;
		myfile.close();

		SDL_SemPost(gDataLock);

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 640);
	}
	return 0;
}
