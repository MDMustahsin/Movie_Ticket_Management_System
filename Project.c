#include <stdio.h>
#include <stdlib.h>

#define MAX_ADMINS 10
#define MAX_USERS 50
#define MAX_NAME 50
#define MAX_PASS 50

int compareString(char a[], char b[])
{
    int i=0;
    while(a[i] != '\0' && b[i] != '\0')
    {
        if(a[i] != b[i])
        {
            return 0;
        }
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

void copyString(char dest[], char src[])
{
    int i=0;
    while(src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

struct Movie
{
    char name[MAX_NAME];
    char time[20];
    int seats;
    struct Movie *next;
};

struct Movie *movieHead = NULL;

struct Ticket
{
    char user[MAX_NAME];
    char movie[MAX_NAME];
    struct Ticket *next;
};

struct Ticket *head = NULL;

struct Ticket *cancelStack[100];
int top = -1;

struct Ticket *waitQueue[100];
int front = 0, rear = -1, qcount = 0;

void enqueue(struct Ticket *t)
{
    if(qcount < 100)
    {
        rear = (rear+1)%100;
        waitQueue[rear] = t;
        qcount++;
    }
}

struct Ticket* dequeue()
{
    if(qcount==0)
    {
        return NULL;
    }
    struct Ticket *t = waitQueue[front];
    front = (front+1)%100;
    qcount--;
    return t;
}

struct Account
{
    char username[MAX_NAME];
    char password[MAX_PASS];
};

struct Account admins[MAX_ADMINS];
int adminCount=0;

struct Account users[MAX_USERS];
int userCount=0;

void signup(int isAdmin)
{
    char uname[MAX_NAME], pass[MAX_PASS];
    printf("Enter username: ");
    scanf("%s", uname);
    printf("Enter password: ");
    scanf("%s", pass);

    if(isAdmin)
    {
        if(adminCount >= MAX_ADMINS)
        {
            printf("Admin list full!\n");
            return;
        }
        copyString(admins[adminCount].username, uname);
        copyString(admins[adminCount].password, pass);
        adminCount++;
        printf("Admin signup successful!\n");
    }
    else
    {
        if(userCount >= MAX_USERS)
        {
            printf("User list full!\n");
            return;
        }
        copyString(users[userCount].username, uname);
        copyString(users[userCount].password, pass);
        userCount++;
        printf("User signup successful!\n");
    }
}

int login(int isAdmin, char uname[])
{
    char pass[MAX_PASS];
    printf("Enter username: ");
    scanf("%s", uname);
    printf("Enter password: ");
    scanf("%s", pass);

    if(isAdmin)
    {
        for(int i=0;i<adminCount;i++)
        {
            if(compareString(admins[i].username, uname) && compareString(admins[i].password, pass))
            {
                printf("Admin login successful!\n");
                return 1;
            }
        }
    }
    else
    {
        for(int i=0;i<userCount;i++)
        {
            if(compareString(users[i].username, uname) && compareString(users[i].password, pass))
            {
                printf("User login successful!\n");
                return 1;
            }
        }
    }
    printf("Login failed!\n");
    return 0;
}

void addMovie()
{
    struct Movie *newMovie = (struct Movie*)malloc(sizeof(struct Movie));
    printf("Enter movie name: ");
    scanf("%s", newMovie->name);
    printf("Enter movie time (e.g. 5:00PM): ");
    scanf("%s", newMovie->time);
    printf("Enter number of seats: ");
    scanf("%d", &newMovie->seats);

    newMovie->next = movieHead;
    movieHead = newMovie;

    printf("Movie added successfully!\n");
}

void viewMovies()
{
    struct Movie *temp = movieHead;
    if(temp == NULL)
    {
        printf("No movies available.\n");
        return;
    }
    int i=1;
    while(temp != NULL)
    {
        printf("%d. %s (%s) - Seats: %d\n", i, temp->name, temp->time, temp->seats);
        temp = temp->next;
        i++;
    }
}

struct Movie* findMovieByIndex(int index)
{
    struct Movie *temp = movieHead;
    int count = 1;
    while(temp != NULL)
    {
        if(count == index) return temp;
        temp = temp->next;
        count++;
    }
    return NULL;
}

void removeMovie()
{
    int choice, count=1;
    struct Movie *temp = movieHead, *prev = NULL;

    printf("\n--- Current Movies ---\n");
    viewMovies();

    printf("Choose movie to remove: ");
    scanf("%d", &choice);

    temp = movieHead;
    count=1;
    while(temp != NULL)
    {
        if(count == choice)
        {
            if(prev == NULL) movieHead = temp->next;
            else prev->next = temp->next;
            free(temp);
            printf("Movie removed successfully!\n");
            return;
        }
        prev = temp;
        temp = temp->next;
        count++;
    }
    printf("Invalid choice!\n");
}

void updateMovie()
{
    int choice;
    char newName[MAX_NAME], newTime[20];
    int newSeats;

    printf("\n--- Current Movies ---\n");
    viewMovies();

    printf("Choose movie to update: ");
    scanf("%d", &choice);

    struct Movie *movie = findMovieByIndex(choice);
    if(movie == NULL)
    {
        printf("Invalid choice!\n");
        return;
    }

    printf("Enter new movie name: ");
    scanf("%s", newName);
    printf("Enter new movie time: ");
    scanf("%s", newTime);
    printf("Enter new seat count: ");
    scanf("%d", &newSeats);

    copyString(movie->name, newName);
    copyString(movie->time, newTime);
    movie->seats = newSeats;

    printf("Movie updated successfully!\n");
}

void bookTicket(char uname[])
{
    int choice;
    struct Movie *temp = movieHead;

    if(temp == NULL)
    {
        printf("No movies available.\n");
        return;
    }

    printf("Available Movies:\n");
    viewMovies();

    printf("Choose movie: ");
    scanf("%d", &choice);

    struct Movie *movie = findMovieByIndex(choice);
    if(movie == NULL) return;

    if(movie->seats > 0)
    {
        struct Ticket *t = (struct Ticket*)malloc(sizeof(struct Ticket));
        copyString(t->user, uname);
        copyString(t->movie, movie->name);
        t->next = head;
        head = t;
        movie->seats--;
        printf("Ticket booked successfully!\n");
    }
    else
    {
        struct Ticket *t = (struct Ticket*)malloc(sizeof(struct Ticket));
        copyString(t->user, uname);
        copyString(t->movie, movie->name);
        enqueue(t);
        printf("All seats booked! You are added to the waiting list.\n");
    }
}

void cancelTicket(char uname[])
{
    struct Ticket *temp = head, *prev=NULL;
    while(temp != NULL)
    {
        if(compareString(temp->user, uname))
        {
            if(prev==NULL) head = temp->next;
            else prev->next = temp->next;

            top++;
            cancelStack[top] = temp;

            struct Movie *m = movieHead;
            while(m != NULL)
            {
                if(compareString(m->name, temp->movie))
                {
                    m->seats++;
                    break;
                }
                m = m->next;
            }

            struct Ticket *waiter = dequeue();
            if(waiter != NULL)
            {
                waiter->next = head;
                head = waiter;
                struct Movie *wm = movieHead;
                while(wm != NULL)
                {
                    if(compareString(wm->name, waiter->movie))
                    {
                        wm->seats--;
                        break;
                    }
                    wm = wm->next;
                }
                printf("Ticket canceled. Waiting list user %s got the seat!\n", waiter->user);
            }
            else
            {
                printf("Ticket canceled.\n");
            }
            return;
        }
        prev=temp;
        temp=temp->next;
    }
    printf("No ticket found for %s.\n", uname);
}

void viewTickets()
{
    struct Ticket *temp = head;
    printf("Current Tickets:\n");
    while(temp!=NULL)
    {
        printf("User: %s | Movie: %s\n", temp->user, temp->movie);
        temp=temp->next;
    }
}

void adminMenu()
{
    int ch;
    do
    {
        printf("\n--- Admin Menu ---\n");
        printf("1. View Tickets\n2. View Movies\n3. Add Movie\n4. Remove Movie\n5. Update Movie\n6. Exit\nChoice: ");
        scanf("%d", &ch);
        if(ch==1)
        {
            viewTickets();
        }
        else if(ch==2)
        {
            viewMovies();
        }
        else if(ch==3)
        {
            addMovie();
        }
        else if(ch==4)
        {
            removeMovie();
        }
        else if(ch==5)
        {
            updateMovie();
        }
    }
    while(ch!=6);
}

void userMenu(char uname[])
{
    int ch;
    do
    {
        printf("\n--- User Menu ---\n");
        printf("1. Book Ticket\n2. Cancel Ticket\n3. View Tickets\n4. Exit\nChoice: ");
        scanf("%d", &ch);
        if(ch==1)
        {
            bookTicket(uname);
        }
        else if(ch==2)
        {
            cancelTicket(uname);
        }
        else if(ch==3)
        {
            viewTickets();
        }
    }
    while(ch!=4);
}

void guestMenu()
{
    int ch;
    do
    {
        printf("\n--- Guest Menu ---\n");
        printf("1. View Movies\n2. Exit\nChoice: ");
        scanf("%d", &ch);
        if(ch==1)
        {
            viewMovies();
        }
    }
    while(ch!=2);
}

int main()
{
    int choice;
    char uname[MAX_NAME];
    while(1)
    {
        printf("\n=== Movie Ticket System ===\n");
        printf("1. Sign Up as Admin\n2. Sign Up as User\n3. Login as Admin\n4. Login as User\n5. Guest Mode\n6. Exit\nChoice: ");
        scanf("%d", &choice);

        if(choice==1)
        {
            signup(1);
        }
        else if(choice==2)
        {
            signup(0);
        }
        else if(choice==3)
        {
            if(login(1, uname))
            {
                adminMenu();
            }
        }
        else if(choice==4)
        {
            if(login(0, uname))
            {
                userMenu(uname);
            }
        }
        else if(choice==5)
        {
            guestMenu();
        }
        else if(choice==6)
        {
            break;
        }
    }
    return 0;
}
