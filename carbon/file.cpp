#include <ncurses.h>

#include <sys/types.h>				//for open()
#include <sys/stat.h>				//for open()
#include <fcntl.h>					//for open()
#include <unistd.h>					//for close(),alarm()
#include <signal.h>
#include <sys/signal.h>

#include <string.h>
#include <stdlib.h>

using namespace std;

#define MAX_USERS 10				// can be a multiple of 10 only

int user_id = 1;

class ListNode {
private:
	int 		id_;				// unique identification of the node
									// unique across all the users
									// last digit signifies the user
	char 		content_;			// character at node
	
	ListNode*	next_;				// next node pointer
	ListNode*	pre_;				// previous node pointer

	static int 	seq_num_;			// unique sequence number

public:
	ListNode(int id);
	ListNode(int id, char content, ListNode*, ListNode*);

													// function overloading
	void			id(int num);					// id_ setter function
	inline int 		id() { return id_; }			// id_ getter function

	void			content(char ch);				// content_ setter function
	inline char 	content() { return content_; }	// content_ getter function

	void 			next(ListNode* ptr);			// next_ setter function
	inline ListNode* next() { return next_; }		// next_ getter function

	void			pre(ListNode* ptr);				// pre_ setter function
	inline ListNode* pre() { return pre_; }			// pre_ getter function

	static int 		getNextSeqNum();				// get next sequence number
													// user id is included
	int 			getUser();						// return the user id
};

int ListNode::seq_num_ = 1;

ListNode::ListNode(int id) {
	id_ = id;
	content_ = '\0';
	next_ = NULL;
	pre_ = NULL;
}

ListNode::ListNode(int id, char content, ListNode* pre = NULL, ListNode* next = NULL) {
	id_ = id;
	content_ = content;
	next_ = next;
	pre_ = pre;
}

void ListNode::id(int num) {
	id_ = num;
}

void ListNode::content(char ch) {
	content_ = ch;
}

void ListNode::next(ListNode* ptr) {
	next_ = ptr;
}

void ListNode::pre(ListNode* ptr) {
	pre_ = ptr;
}

int ListNode::getNextSeqNum() {
	return (seq_num_++ * MAX_USERS + user_id);
}

int ListNode::getUser() {
	return id_ % MAX_USERS;
}

class List {
private:
	ListNode* 	start_;
	ListNode*	current_;
	ListNode*	end_;

public:
	List();

	ListNode*	initCursor();

	int 		movePre();
	int 		movePre(ListNode*);

	int 		moveNext();
	int 		moveNext(ListNode*);

	void		insert(char content);							// Insert at current position 
																// of ptr with auto generated id 
	void		insert(int id, int after_id, char content);		// insert after after_id 

	void		backspace();
	void		del();
	void		del(int id);

	void		print();
};

List::List() {
	start_ = NULL;
	current_ = '\0';
	end_ = NULL; 
}

ListNode* List::initCursor() {
	ListNode* newcursor;
	newcursor = start_;
	return newcursor;
}

int List::movePre() {
	if(current_->pre() != NULL){
		current_ = current_->pre();
		return 1;
	}
	return 0;
}

int List::movePre(ListNode* cur) {
	if(cur->pre() != NULL) {
		cur = cur->pre();
		return 1;
	}
	return 0;
}

int List::moveNext() {
	if(current_->next() != NULL) {
		current_ = current_->next();
		return 1;
	}
	return 0;
}

int List::moveNext(ListNode* cur) {
	if(cur->next() != NULL) {
		cur = cur->next();
		return 1;
	}
	return 0;
}

void List::insert(char content) {
	if(!start_) {
		start_ = new ListNode(ListNode::getNextSeqNum(), content);
		current_ = end_ = start_;
	}
	else {
		current_->next(new ListNode(ListNode::getNextSeqNum(), content, current_,current_->next()));
		current_ = current_->next();
		
		if(current_->next()) {
			current_->next()->pre(current_);
		}

		if(current_ == end_) {
			end_ = current_;
		}
	}
}

void List::insert(int id, int after_id, char content) {
	ListNode* cur;
	cur = start_;
	while(cur) {
		if(cur->id() == after_id) {
			cur->next(new ListNode(id, content, cur, cur->next()));
			cur = cur->next();

			if(cur == end_) {
				end_ = cur;
			}

			break;
		}
		cur = cur->next();
	}
}

void List::backspace() {
	ListNode* ptr;
	ptr = current_;

	if(current_ != start_){
		current_->pre()->next(current_->next());

		if(current_->next()){
			current_->next()->pre(current_->pre());
		}

		if(current_ == end_){
			current_ = end_ = current_->pre();
		}
		else{
			current_ = current_->pre();
		}	
	}
	else {
		if(start_->next()) {
			start_ = start_->next();
		}
		if(start_) {
			start_->pre(NULL);
		}
		current_ = start_;
	}

	delete ptr;
}

void List::del() {
	ListNode* ptr;
	ptr = current_->next();

	current_->next(ptr->next());
	ptr->next()->pre(current_);

	if(end_ == ptr) {
		end_ = current_;
	}

	delete ptr;
}

void List::del(int id) {
	ListNode* ptr;
	ptr = start_;
	
	while(ptr) {
		if(ptr->id() == id) {
			ptr->pre()->next(ptr->next());
			ptr->next()->pre(ptr->pre());
			delete ptr;
			break;
		} 
		ptr = ptr->next();
	}
}

void List::print() {
	ListNode* ptr;
	for(ptr=start_; ptr != NULL; ptr = ptr->next()) {
		printw("%c",ptr->content());
	}
}

class UI {

	int x;
	int y;
public:
	void init();
	void paint(List parselist);
	void repaint(List parselist);
	void mvCursorLeft();
	void mvCursorRight();
	void close();
};

void UI::init() {
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	//raw();
	cbreak();
}
void UI::paint(List parselist) {
	clear();
	parselist.print();
	refresh();
}

void UI::repaint(List parselist) {
	getyx(stdscr, y, x);
	clear();
	parselist.print();
	move(y, x);
	refresh();
}

void UI::close() {
	endwin();
}

void UI::mvCursorLeft() {
	getyx(stdscr, y, x);
	move(y, x-1);
}

void UI::mvCursorRight() {
	getyx(stdscr, y, x);
	move(y, x+1);
}

int main(int argc, char *argv[])
{	

	int fd;

	if(argc < 3) {
		perror("Usage: <file.out> <open/create> <filename>");
		exit(0);
	}

	if(strcmp(argv[1],"create")==0) {
		// create the file
		if((fd = creat(argv[2], 0660))==-1) {
			perror("create");
			exit(1);
		}
	}
	else if(strcmp(argv[1],"open")==0) {
		// open the file
		if((fd = open(argv[2], O_RDONLY))==-1) {
			perror("open");
			exit(1);
		} 
	}
	else {
		perror("Invalid command");
		exit(1);
	}

	List parselist;
	UI interface;
	int n;
	char *buf;
	buf = new char;

	while((n=read(fd, buf, sizeof(char))) > 0) {
		printf("%c",*buf);
		parselist.insert(*buf);
	}
	close(fd);		// reading from file is completed

	interface.init();
	interface.paint(parselist);	   

	// Important bug fix
	// http://www.cplusplus.com/forum/unices/93548/#msg502424
	wchar_t hit;
	while(1) {
		hit = getch();

		switch(hit) {
			case KEY_LEFT:
							if(parselist.movePre()) {
								interface.mvCursorLeft();
							}
							break;
			case KEY_RIGHT:
							if(parselist.moveNext()) {
								interface.mvCursorRight();
							}
							break;
			case KEY_UP:
							break;
			case KEY_DOWN:
							break;
			case KEY_BACKSPACE:
							parselist.backspace();
							interface.mvCursorLeft();
							interface.repaint(parselist);
							break;
			default:			
						parselist.insert(hit);
						interface.mvCursorRight();
						interface.repaint(parselist);
		}
		if(hit == KEY_LEFT) {
			
		}
		else if(hit == KEY_RIGHT) {
			
		}
		else {
		}
	}

	interface.close();
	
	close(fd);
	return 0;
}