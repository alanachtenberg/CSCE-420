/* PANDAMAT

file PAND4.CPP

by Jeff Whitledge

Fall 1995

Pandemonium controlling an animat.
*/

/*-------------------------- Cursor positioning code added by rgw ---------------*/
#define _CRT_NONSTDC_NO_WARNINGS //preprocessor definitions for legacy code to run on visual studio 2013
#define _CRT_SECURE_NO_WARNINGS //disables Std C and POSIX deprecation warnings repectively
#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

void gotoxy(int x, int y)
{
	//Initialize the coordinates
	COORD coord = { x - 1, y - 1 };	//Pandamat coordinates origin is 1,1, whereas Windows is 0,0
	//Set the position
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	return;
}
void clrscr()
{
	//Get the handle to the current output buffer...
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//This is used to reset the carat/cursor to the top left.
	COORD coord = { 0, 0 };
	//A return value... indicating how many chars were written
	//   not used but we need to capture this since it will be
	//   written anyway (passing NULL causes an access violation).
	DWORD count;
	//This is a structure containing all of the console info
	// it is used here to find the size of the console.
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//Here we will set the current color
	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		//This fills the buffer with a given character (in this case 32=space).
		FillConsoleOutputCharacter(hStdOut, (TCHAR)32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
		FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
		//This will set our cursor position for the next print statement.
		SetConsoleCursorPosition(hStdOut, coord);
	}

	return;
}

/*-------------------------- Jeff Whitledge's original Pandamat code below ---------------*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
//#include <values.h>
//#include <graphics.h>
#include <dos.h>


#define MAXX 58     //defines the width and highth of the landscape
#define MAXY 19

#define N 50            // the number of problems to display the average of
#define GRAPH_SIZE_X 8000L  // the number of problems to graph
#define GRAPH_SIZE_Y 20L    // the maximum problem time to graph
#define GRANULARITY 5   // the width of each graph item in pixels

int graph_mode = 0;    // 0 is text mode,  1 is graphics
int speed = 0;         // 0 is fast,       1 is slow

/*-------------------------- Landscape (the ANIMAT portion)---------------*/


typedef enum
{
	north,
	ne,
	east,
	se,
	south,
	sw,
	west,
	nw
} direction_type;
#define FIRST_DIRECTION north
#define MAX_DIRECTION nw
//operator overload added by rgw
direction_type &operator++(direction_type &en) {
	if (en == MAX_DIRECTION) { en = FIRST_DIRECTION; return en; }  //wrap
	en = static_cast<direction_type>(static_cast<int>(en)+1); //increment
	return en;
}

direction_type &operator--(direction_type &en) {
	if (en == FIRST_DIRECTION) { en = MAX_DIRECTION; return en; }  //wrap
	en = static_cast<direction_type>(static_cast<int>(en)-1); //decrement
	return en;
}



typedef enum
{
	none_f = ' ',
	tree_f = 'T',
	food_f = 'F'
} feature_type;


typedef char sense_vector[9];

// "gain" and "senses" are passed from
// the environment (animat) to the control structure (pandemonium)
float gain;
sense_vector senses;

// these variables keep statistics
int problem_time;
int last_time;
unsigned long int problem_number;
int last_N[N];
int current;
int graph_data[GRAPH_SIZE_X];
long int max_x, max_y;


// the land_c class is the environment
class land_c
{
private:

	char field[MAXY][MAXX + 1];
	int animatx, animaty;

	void erase_animat();
	void draw_animat();
	void move_animat_randomly();
	void print_data();
	void eat_food(int newx, int newy);

public:

	land_c();
	void draw_field();
	void move_animat(direction_type d);

};


land_c::land_c()
{
	// the environment is WOODS07
	strcpy(field[0], "          T                          TT         T         ");
	strcpy(field[1], "TFT       F         F         T       F         FT        ");
	strcpy(field[2], "          T         TT       F                            ");
	strcpy(field[3], "                            T      T         F     T      ");
	strcpy(field[4], "          TFT         TFT          F         TT     F     ");
	strcpy(field[5], "  F                                T                 T    ");
	strcpy(field[6], "  TT                        TT                    T       ");
	strcpy(field[7], "         TT        T         F         TFT        F       ");
	strcpy(field[8], "TFT       F        TF                             T       ");
	strcpy(field[9], "                              T        TT       T         ");
	strcpy(field[10], "  TT       T                 FT        F        TF   TFT  ");
	strcpy(field[11], "  F       F        T                                      ");
	strcpy(field[12], "          T        TF                 T      T      T     ");
	strcpy(field[13], " T                       T           FT       F     TF    ");
	strcpy(field[14], " F       F                F                    T          ");
	strcpy(field[15], " T       TT                T                              ");
	strcpy(field[16], "                     T                     T              ");
	strcpy(field[17], "  F        TFT       F          F         F      TF       ");
	strcpy(field[18], "  TT                 T          TT       T       T        ");

	animatx = 1;
	animaty = 1;
	move_animat_randomly();
	problem_time = 0;

}


void land_c::draw_field()
{
	int index;

	clrscr();

	for (index = 0; index<MAXY; index++)
		puts(field[index]);
	return;

}

void land_c::erase_animat()
{
	if (graph_mode) return;
	gotoxy(animatx + 1, animaty + 1);		//why the +1 offset???
	putch(field[animaty][animatx]);	//y and x are not backwards
	return;
}

void land_c::draw_animat()
{
	if (graph_mode) return;
	gotoxy(animatx + 1, animaty + 1);	//why the +1 offset???
	putch('*');
	return;
}

void land_c::print_data()
{
	float average = 0.0; //bug
	int index;

	problem_number++;
	last_time = problem_time;
	problem_time = 0;
	last_N[current++] = last_time;
	if (current == N) current = 0;
	for (index = 0; index<N; index++)
		average += (float)last_N[index];
	average /= N;
	if (problem_number<GRAPH_SIZE_X) graph_data[problem_number] = last_time;

	if (!graph_mode)
	{
		gotoxy(50, 20);
		printf("problem %lu     ", problem_number);
		gotoxy(50, 21);
		printf("last problem time: %d    ", last_time);
		if (problem_number>N)
		{
			gotoxy(50, 22);
			printf("average over last %d: %2.2f  ", N, average);
		}
	}
	else
	{
		// in graphics mode.  Draw nothing.
	}
}

void land_c::move_animat_randomly()
{
	erase_animat();
	do
	{
		//random(n) function call replaced by rgw
		animatx = 1.0 * (MAXX - 1) * rand() / RAND_MAX; //random(MAXX); //random(N) returns a random number from 0 to N-1
		animaty = 1.0 * (MAXY - 1) * rand() / RAND_MAX;//random(MAXY);
	} while (field[animaty][animatx] != none_f);	//keep moving until we hit a blank
	draw_animat();
}


void land_c::eat_food(int newy, int newx)
{
	if (speed == 0) return;
	gotoxy(newx + 1, newy + 1);	//why + 1??
	//textcolor(YELLOW);		//commented out by rgw for the time being
	putch('X');
	//delay(ms) call replaced by rgw
	Sleep(500);      //delay(500);
	gotoxy(newx + 1, newy + 1);
	//textcolor(LIGHTGRAY);	//commented out by rgw for the time being
	putch('F');

}

bool treeFound = false;//a little bit of memory for our animat, has a tree been found?
direction_type treeDirection;//direction the last tree was found in
void land_c::move_animat(direction_type d)
{
	int newx = 0, newy = 0;

	problem_time++;

	if (treeFound == true){
		switch (treeDirection){ //where the magic happens, this overides usual logic once a tree has been found
		case north:			  //new logic walks pandamat around the tree until it either finds another tree or food
			d = direction_type::east;//move east
			treeDirection = direction_type::nw;//new tree direction, releative to new position
			break;
		case ne:
			d = direction_type::east;//move east
			treeDirection = direction_type::north;
			break;
		case east:
			d = direction_type::south;//move south
			treeDirection = direction_type::ne;
			break;
		case se:
			d = direction_type::south;//move south
			treeDirection = direction_type::east;
			break;
		case south:
			d = direction_type::west;//move west
			treeDirection = direction_type::se;
			break;
		case sw:
			d = direction_type::west;//move west
			treeDirection = direction_type::south;
			break;
		case west:
			d = direction_type::north;//move north
			treeDirection = direction_type::sw;
			break;
		case nw:
			d = direction_type::north;//move north
			treeDirection = direction_type::west;
			break;
		}
	}

	switch (d)
	{
	case nw:
	case north:
	case ne:
		newy = animaty - 1;
		break;
	case west:
	case east:
		newy = animaty;
		break;
	case sw:
	case south:
	case se:
		newy = animaty + 1;
		break;
	}

	switch (d)
	{
	case nw:
	case west:
	case sw:
		newx = animatx - 1;
		break;
	case north:
	case south:
		newx = animatx;
		break;
	case ne:
	case east:
	case se:
		newx = animatx + 1;
		break;
	}
	if (newx>MAXX - 1) newx = 0;
	if (newx<0) newx = MAXX - 1;
	if (newy>MAXY - 1) newy = 0;
	if (newy<0) newy = MAXY - 1;
	//switch(senses[d])
	switch (field[newy][newx])	//move is finalized based on current field value, why not senses[] value??
	{
	case none_f:
		erase_animat();
		animatx = newx;
		animaty = newy;
		draw_animat();

		gain = -0.25; //-0.01 -0.25
		break;
	case tree_f:		//no movement in case of a tree
		treeFound = true;//we found a tree! lets remember that
		treeDirection = d;//lets remember which direction that was as well
		gain = -1.0;
		break;
	case food_f:
		treeFound = false;//reset for next problem
		eat_food(newy, newx);
		gain = 2.25;  // 0.2   2.0
		move_animat_randomly();
		print_data();
		break;
	}
	//update the sensors based on new animat position
	senses[0] = field[(MAXY + animaty - 1) % MAXY][animatx]; //n
	senses[1] = field[(MAXY + animaty - 1) % MAXY][(animatx + 1) % MAXX]; //ne
	senses[2] = field[animaty][(animatx + 1) % MAXX]; //e
	senses[3] = field[(animaty + 1) % MAXY][(animatx + 1) % MAXX]; //se
	senses[4] = field[(animaty + 1) % MAXY][animatx]; //s
	senses[5] = field[(animaty + 1) % MAXY][(MAXX + animatx - 1) % MAXX]; //sw
	senses[6] = field[animaty][(MAXX + animatx - 1) % MAXX]; //w
	senses[7] = field[(MAXY + animaty - 1) % MAXY][(MAXX + animatx - 1) % MAXX]; //nw


	return;
}


land_c land;


/*---------------------- Control Structure (PANDEMONIUM) ---------------*/

/* There are three types of demons: sense, action, and other.
The sense demons get their volumes from the environment.
The action demons effect the environment.
The other demons don't do anything in particular.  */

#define NUM_SENSE 24	//rgw comment: number of sense demons tightly interlocked with the code and increment of g_direction in the sense demon constructor
#define NUM_OTHER 10
#define NUM_ACTION 8
#define NUM_DEMONS NUM_SENSE+NUM_OTHER+NUM_ACTION
#define ARENA_SENSE  8
#define ARENA_OTHER  2
#define ARENA_ACTION 1
#define SIZE_ARENA ARENA_SENSE+ARENA_OTHER+ARENA_ACTION


int arena[SIZE_ARENA];
float connections[NUM_DEMONS][NUM_DEMONS];

// the class demon is the base class for all of the demons
class demon
{
protected:
	float volume;            // how load the demon is shouting
	float built_in_volume;   // default base volume for this demon
	int my_number;
public:
	demon();
	float get_volume() { return volume; }
	virtual void calc_volume();
	void get_number(int num);
	void adjust_strengths();  // adjusts the volume connections
	// according to the gain
	virtual void print_stuff();  //used for debugging
};

demon::demon()
{
}

void demon::calc_volume()
{
	int index;

	volume = built_in_volume;
	for (index = 0; index<SIZE_ARENA; index++)
		volume += connections[my_number][arena[index]];
	return;
}

void demon::get_number(int num)
{
	my_number = num;
	//random(n) call replaced by rgw
	built_in_volume = (100.0 - 1.0) / 10.0 * rand() / RAND_MAX; //(float)random(100)/10;////random(N) returns a random number from 0 to N-1
	return;
}

void demon::adjust_strengths()
{
	int index;

	built_in_volume += gain / 10.0;
	for (index = 0; index<SIZE_ARENA; index++)
		connections[my_number][arena[index]] += gain;
	return;
}

void demon::print_stuff()
{
	fprintf(stdout, "other:%d,volume=%f\n", my_number, volume);
}

feature_type g_feature = none_f;          // used in constructing sense demons
direction_type g_direction = FIRST_DIRECTION;

class sense_demon :demon
{
private:
	direction_type direction;
	feature_type feature;
public:
	sense_demon();
	virtual void calc_volume();
	virtual void print_stuff();
};

sense_demon::sense_demon()
{
	direction = g_direction;
	feature = g_feature;
	//rgw comment: this constructor is tightly tied to external variables
	if ((g_direction++) == MAX_DIRECTION)  //time to switch init to next feature type? starts with none_f (blank)
	{
		g_direction = FIRST_DIRECTION; //the next 8 sense demons will correspond to a new feature
		switch (g_feature)
		{
		case none_f:
			g_feature = tree_f; break;	//if feature was a blank, then tree sense demons are next
		case tree_f:
			g_feature = food_f;			//food feature demons are last
		}
	}
}

void sense_demon::calc_volume()
{
	int index;

	//volume=built_in_volume;
	volume = 0.0;
	if (senses[direction] == feature) volume += 10.0;
	// for (index=0;index<SIZE_ARENA;index++)
	//   volume+=connections[my_number][arena[index]];
}

void sense_demon::print_stuff()
{
	fprintf(stdout, "sense: %d,direction=%d,feature=%d,volume=%f\n", my_number, direction, feature, volume);
}

direction_type ga_direction = FIRST_DIRECTION;

class action_demon :demon
{
private:
	direction_type direction;
public:
	action_demon();
	void take_action();
	virtual void calc_volume();
	virtual void print_stuff();
};

action_demon::action_demon()
{
	direction = ga_direction; //rgw comment: bug fix due the way action demons are constructed. Made assignment first to direction, then incremented the global ga_direction
	ga_direction++;
}

void action_demon::take_action()
{
	//gotoxy(1,20);
	//printf("moving %d   time %d   \n",direction,problem_time);
	land.move_animat(direction);
}

void action_demon::calc_volume()
{
	int index;

	volume = built_in_volume;
	for (index = 0; index<SIZE_ARENA; index++)
		volume += connections[my_number][arena[index]];
	return;
}

void action_demon::print_stuff()
{
	fprintf(stdout, "action: %d, direction=%d, volume=%f\n", my_number, direction, volume);
}

sense_demon  sense_demons[NUM_SENSE];
demon        other_demons[NUM_OTHER];
action_demon action_demons[NUM_ACTION];

demon *all_demons[NUM_DEMONS];


void iterate_pandemonium(void)
{
	int index, arena_index;
	float arena_volumes[NUM_DEMONS];
	int current_demon, temp_demon;
	float current_volume, temp_vol;

	for (index = 0; index<NUM_DEMONS; index++) arena_volumes[index] = -300000.0;

	//calculate volumes for sense demons
	for (index = 0; index<NUM_SENSE; index++)
		sense_demons[index].calc_volume();

	//move loudest sense demons into the arena
	for (index = 0; index<NUM_SENSE; index++)
	{
		current_demon = index;
		current_volume = all_demons[index]->get_volume();
		for (arena_index = 0; arena_index<ARENA_SENSE; arena_index++)
		{
			if (current_volume>arena_volumes[arena_index])
			{
				temp_vol = arena_volumes[arena_index];
				temp_demon = arena[arena_index];
				arena_volumes[arena_index] = current_volume;
				arena[arena_index] = current_demon;
				current_volume = temp_vol;
				current_demon = temp_demon;
			}
		}
	}

	//calculate volumes for the other demons
	for (index = 0; index<NUM_OTHER; index++)
		other_demons[index].calc_volume();

	//move loudest other demons into the arena
	for (index = NUM_SENSE; index<NUM_SENSE + NUM_OTHER; index++)
	{
		current_demon = index;
		current_volume = all_demons[index]->get_volume();
		for (arena_index = ARENA_SENSE; arena_index<ARENA_SENSE + ARENA_OTHER; arena_index++)
		{
			if (current_volume>arena_volumes[arena_index])
			{
				temp_vol = arena_volumes[arena_index];
				temp_demon = arena[arena_index];
				arena_volumes[arena_index] = current_volume;
				arena[arena_index] = current_demon;
				current_volume = temp_vol;
				current_demon = temp_demon;
			}
		}
	}


	//calculate volumes for action demons
	for (index = 0; index<NUM_ACTION; index++)
		action_demons[index].calc_volume();

	//move loudest action demon into the arena
	for (index = NUM_SENSE + NUM_OTHER; index<NUM_DEMONS; index++)
	{
		current_demon = index;
		current_volume = all_demons[index]->get_volume();
		arena_index = ARENA_SENSE + ARENA_OTHER;
		if (current_volume>arena_volumes[arena_index])
		{
			temp_vol = arena_volumes[arena_index];
			temp_demon = arena[arena_index];
			arena_volumes[arena_index] = current_volume;
			arena[arena_index] = current_demon;
			current_volume = temp_vol;
			current_demon = temp_demon;
		}
	}

	//take action
	action_demons[arena[ARENA_SENSE + ARENA_OTHER] - NUM_SENSE - NUM_OTHER].take_action();
	//gotoxy(1,21);   // used for debugging
	//for (index=0;index<SIZE_ARENA;index++)
	//  printf(" %d %2.2f\t",arena[index],arena_volumes[index]);

	//change connections according to fitness
	for (index = 0; index<SIZE_ARENA; index++)
		all_demons[arena[index]]->adjust_strengths();

	return;
}

void initialize_pandemonium()
{
	int index1, index2;

	for (index1 = 0; index1<NUM_DEMONS; index1++)
		for (index2 = 0; index2<NUM_DEMONS; index2++)
			connections[index1][index2] = (100.0 - 1.0) / 10.0 * rand() / RAND_MAX; //(float)random(100)/10;

	for (index1 = 0; index1<NUM_SENSE; index1++)
		all_demons[index1] = (demon *)&sense_demons[index1];
	for (index1 = 0; index1<NUM_OTHER; index1++)
		all_demons[index1 + NUM_SENSE] = &other_demons[index1];
	for (index1 = 0; index1<NUM_ACTION; index1++)
		all_demons[index1 + NUM_SENSE + NUM_OTHER] = (demon *)&action_demons[index1];
	for (index1 = 0; index1<NUM_DEMONS; index1++)
		all_demons[index1]->get_number(index1);
}


/*---------------------- main ------------------------------------------*/

void make_graphics()	//graphics code commented out by rgw
{
	/*  int index,current_x,count;
	float average;
	int gdriver = DETECT, gmode, errorcode;

	initgraph(&gdriver, &gmode, "");
	errorcode = graphresult();
	if (errorcode != grOk)  /* an error occurred */
	/*   {
	gotoxy(1,23);
	printf("Graphics error: %s\n", grapherrormsg(errorcode));
	graph_mode=0;
	return;
	}

	max_x=getmaxx();
	max_y=getmaxy();

	line(0,0,0,max_y);
	line(0,max_y,max_x,max_y);
	for (index=0;index<GRAPH_SIZE_Y;index+=5)
	line( 0, max_y-max_y/(float)GRAPH_SIZE_Y*index,
	10, max_y-max_y/(float)GRAPH_SIZE_Y*index);
	for (index=0;index<GRAPH_SIZE_X;index+=1000)
	line(max_x/(float)GRAPH_SIZE_X*index,max_y,
	max_x/(float)GRAPH_SIZE_X*index,max_y-10);
	index=0;
	for (current_x=0;current_x<problem_number*max_x/(float)GRAPH_SIZE_X;current_x+=GRANULARITY)
	{
	average=0.0;
	for (count=0;index<current_x*GRAPH_SIZE_X/(float)max_x;index++,count++)
	{
	if (index>GRAPH_SIZE_X) break;
	average+=graph_data[index];
	}
	if (count==0) continue;
	if (index>GRAPH_SIZE_X) break;
	average/=count;
	lineto(current_x,max_y-max_y/(float)GRAPH_SIZE_Y*average);
	}
	*/
	/* for (index=2;index<problem_number;index++)
	line(max_x/(float)GRAPH_SIZE_X*index,
	max_y-max_y/(float)GRAPH_SIZE_Y*graph_data[index],
	max_x/(float)GRAPH_SIZE_X*(index-1),
	max_y-max_y/(float)GRAPH_SIZE_Y*graph_data[index-1]);*/

}

void make_text()
{
	//closegraph();
	//_setcursortype(_NOCURSOR);
	land.draw_field();
}


int main()
{
	int key;

	//_setcursortype(_NOCURSOR);		//commented out by rgw for the time being
	srand((unsigned)time(0)); //randomize();
	land.draw_field();
	initialize_pandemonium();

	//for (key=0;key<NUM_DEMONS;key++)
	//  all_demons[key]->print_stuff();

	key = 0;
	while (key != 27) //not escape
	{
		if (speed == 1)
			//delay(ms) call replaced by rgw
			Sleep(100);      //delay(100);
		iterate_pandemonium();
		key = 0;
		if (kbhit()) key = getch();
		if ((key == 'G') || (key == 'g'))
			switch (graph_mode)
		{
			case 0:
				graph_mode = 1;
				make_graphics();
				break;
			case 1:
				graph_mode = 0;
				make_text();
				break;
		}
		if ((key == 's') || (key == 'S'))
			speed = 1 - speed;
	}

	if (graph_mode) make_text();
	gotoxy(1, 24);
	//_setcursortype(_NORMALCURSOR);	//commented out by rgw for the time being
	return 0;
}