// ANTISORT v 0.4b (c) 2001 Graham Mitchell - http://grahammitchell.net/
//  6 July 2001
// Reorders the lines of a text file, sending them to stdout or a file.

/*
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License (LICENSE.TXT) for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#define DEBUG 0

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>
#include <iostream.h>

typedef char* char_ptr;

int my_getline(istream& is, char* & s, char terminator = '\n');


int main( int argc, char* argv[] )
{
	cerr << "ANTISORT v0.4b (c) 2001 Graham Mitchell - http://grahammitchell.net/\n";
	// Check parameters
	if ( argc < 2 )
	{
		cerr << "Usage: ANTISORT filename [outfile]\n";
		return 1;
	}

	// Open input file
	ifstream in(argv[1]);
	if ( !in.good() )
	{
		cerr << "Unable to open file \"" << argv[1] << "\"\n";
		return 2;
	}

	srand( (unsigned) time(NULL) );

	/* Here's the plan.
	Use a dynamically-allocated array (so it can grow as large as needed).
	Each array slot is a pointer to an array of chars containing a line from
	the file.  (We use pointers so we don't have to copy them around when
	we "sort".)

	Once all the lines are read in, we "sort" the array.  And then we can
	just output them in the new, random order.  Simple.
	*/

	// Set up our variables.
	char_ptr* lines;
	char_ptr current = NULL;
	int max = 100, used=0, i;
	lines = new char_ptr[max];

	// Read in all the lines
#if DEBUG
	cerr << "Reading in lines...";
	time_t start, stop;
	start = time(NULL);
#endif

	while ( my_getline(in,current,'\n') )
	{
		if ( used == max )
		{
			// array is full, grow (double capacity)
			max *= 2;
			char_ptr* temp = new char_ptr[max];
			for ( i=0; i<used; ++i )
				temp[i] = lines[i];
			delete [] lines;
			lines = temp;
		}
		lines[used] = current;
		++used;
	}
	in.close();
#if DEBUG
	stop = time(NULL);
	cerr << "done (" << (stop-start) << " seconds)\n";
#endif

	delete [] current;	// not a huge memory leak, but it's the Right Thing To Do.


	// Now "sort" the array.  We do one pass forward and then another
	// backward because it smooths out the distribution somehow.

#if DEBUG
	start = stop;
	cerr << "\"Sorting\"...";
#endif
	int r;

	if ( used > 1 )  // only rearrange if there's more than one line
	{
		// Forward
		r = rand()%used;
		current = lines[r];
		for ( i=0; i<used; ++i )
		{
			lines[r] = lines[i];
			r = rand()%used;
			lines[i] = lines[r];
		}
		lines[r] = current;

		// and then Backward
		r = rand()%used;
		current = lines[r];
		for ( i=used-1; i>=0; --i )
		{
			lines[r] = lines[i];
			r = rand()%used;
			lines[i] = lines[r];
		}
		lines[r] = current;
	}
#if DEBUG
	stop = time(NULL);
	cerr << "done (" << (stop-start) << " seconds)\n";
#endif

	// Should be randomized, so just output them.
#if DEBUG
	start = stop;
	cerr << "Outputting...";
#endif
	if ( argc > 2 )
	{
		// Output is to a file
		ofstream out;
		out.open(argv[2]);
		for ( i=0; i<used; ++i )
			out << lines[i] << '\n';
		out.close();
	}
	else
	{
		// Output is to the screen
		for ( i=0; i<used; ++i )
			cout << lines[i] << '\n';
	}
#if DEBUG
	stop = time(NULL);
	cerr << "done (" << (stop-start) << " seconds)\n";
	start = stop;
	cerr << "Cleaning up memory...";
#endif

	// Clean up...
	for ( i=0; i<used; ++i )
		delete [] lines[i];
	delete [] lines;
#if DEBUG
	stop = time(NULL);
	cerr << "done (" << (stop-start) << " seconds)\n";
#endif

	// ...and go home.
	return 0;
}



int my_getline(istream& is, char* & s, char terminator)
{
	// Reads characters into 's' from 'is', until 'terminator' is seen.
	// Terminator is removed from the stream but not inserted in 's'.

	// Unlike the standard getline, 's' is not assumed to have any space
	// allocated at all, and will grow as necessary.

	// Returns number of characters read, including terminator.

	int capacity = 80;
	int used = 0;
	char* temp;
	char c=0;

	s = new char[capacity+1];

	while ( true )
	{
		is.get(c);

		if ( is.eof() || c == terminator )
		{
			s[used] = '\0';
			// if there's much slack, shrink the char[] to just large enough
			if ( capacity-used > 7 || capacity-used < -7 )
			{
				temp = new char[used+1];
				strcpy(temp,s);
				delete [] s;
				s = temp;
			}
			if ( c == terminator )
				return used+1;
			else
				return used;
		}

		if ( used == capacity )
		{
			// grow
			capacity *= 2;
			temp = new char[capacity+1];
			strncpy(temp,s,used);
			delete [] s;
			s = temp;
		}

		s[used] = c;
		++used;
	}

}

