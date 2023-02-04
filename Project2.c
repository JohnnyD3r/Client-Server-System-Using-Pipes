#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_CLIENTS 10 // Maximum Number Of Clients
#define MAX_MOVIES 10 // Maximum Number Of Movies
#define MAX_SEATS 5 // Maximum Number Of Seats Per Movie
#define SERVICE_TIME 500000 // Time To Process Booking Request (In Microseconds) = 0.5 Seconds

// Structure To Represent A Movie
struct movie {
  int id; // Movie ID
  int start; // Start Time
  int end; // End Time
  int available_seats; // Number Of Available Seats
  int price; // Ticket Price
  int successful; // Successful Requests
  int unsuccessful; // Unsuccessful Requests
};

// Initialize The Movies Table With Random Values
void init_movies(struct movie *movies) {
  srand(time(NULL)); // Seed The Random Number Generator
  for (int i = 0; i < MAX_MOVIES; i++) {
    movies[i].id = i;
    movies[i].start = rand() % 22; // Start Time Is A Random Number Between 0 And 21
    movies[i].end = movies[i].start + rand() % 2 + 1; // End Time Is Start Time Plus A Random Number Between 1 And 2
    movies[i].available_seats = rand() % MAX_SEATS + 1; // Available Seats Is A Random Number Between 1 And 5
    movies[i].price = (rand() % 9 + 7); // Ticket Price Is A Random Number Between 7 And 15
    // Additional Values
    movies[i].successful = 0; // Initialize Successful Requests to 0
    movies[i].unsuccessful = 0; // Initialize Unsuccessful Requests to 0
  }
}

// Print The Movies Table
void print_movies(struct movie *movies) {
  printf("ID\tStart\tEnd\tAvailable Seats\t\tPrice\n");
  for (int i = 0; i < MAX_MOVIES; i++) {
    printf("%d\t%.2d:00\t%.2d:30\t%d\t\t\t%d\n", movies[i].id, movies[i].start, movies[i].end, movies[i].available_seats, movies[i].price);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {

  // Movies Table
  printf("\n\t============ Movies ============\n");
  struct movie movies[MAX_MOVIES];
  init_movies(movies); // Initializing Movies Table Through Function
  print_movies(movies); // Print Movies Table Through Function

  // Pipes For Communication With Clients
  int request_pipes[MAX_CLIENTS][2]; // Pipes For Client Requests
  int response_pipes[MAX_CLIENTS][2]; // Pipes For Server Responses

  // Create Pipes For Communication With Clients
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (pipe(request_pipes[i]) == -1) {
      perror("Error Creating Pipe For Client Requests");
      return 1;
    }
    if (pipe(response_pipes[i]) == -1) {
      perror("Error Creating Pipe For Server Responses");
      return 1;
    }
  }

  // Create Child Processes For Clients
  
  for (int i = 0; i < MAX_CLIENTS; i++) {
    srand(i + time(0)); // Initialize The Pseudorandom Number Generator
    int pid = fork();
    if (pid == 0) {
      // Child Process (Client)
      // Close Unused Ends Of Pipes
      close(request_pipes[i][0]); // Close Read End Of Request Pipe
      close(response_pipes[i][1]); // Close Write End Of Response Pipe

      // Send Request To Server
      int num_tickets = rand() % 3 + 1; // Number Of Tickets To Request
      int movie_id = rand() % MAX_MOVIES; // ID Of Movie To Request Tickets
      char request[100]; // 'request' Is Defined With A Size Of 100 Characters
      sprintf(request, "%d %d", num_tickets, movie_id); // Writes The String To A Character Array.
                                                        // In This Case, The Formatted String Written To The 'request' Array 
                                                        // Is Of The Form "<num_tickets> <movie_id>"
      if (write(request_pipes[i][1], request, strlen(request) + 1) == -1) { // Used To Write The Request Array To A Pipe With A File Descriptor Of 'request_pipes[i][1]'
        perror("Error Writing To Request Pipe"); 
        return 1; // Code Returns A Value Of 1, Indicating That An Error Occurred.
      }

      // Wait For Response From Server
      char response[100]; // 'response' Is Defined With A Size Of 100 Characters
      if (read(response_pipes[i][0], response, 100) == -1) { // If The 'read' Function Is Successful, It Will Return The Number Of Characters Read
                                                             // And The Data Will Be Stored In The 'response' Array.
        perror("Error Reading From Response Pipe");
        return 1;
      }
      printf("Client %d: %s\n", i, response); // Print: Client (Client Number): (Success/Failed)
      if (strcmp(response, "Failed") == 0) { // If 'response' = "Failed" (strcmp = String Compare)
        printf("We Couldn't Book %d Tickets For Movie With ID: %d\n", num_tickets, movie_id); // Print Successful Booking Message 
      } else if (strcmp(response, "Success") == 0){ // Else, If 'response' = "Success" 
        printf("We Booked %d Tickets For Movie With ID: %d\n", num_tickets, movie_id); // Print Unsuccessful Booking Message 
      }
      
      // Close Pipes And Exit
      close(request_pipes[i][1]);
      close(response_pipes[i][0]);
      return 0;
    } else if (pid > 0) {
      // Parent Process (Server)
      // Close Unused Ends Of Pipes
      close(request_pipes[i][1]); // Close Write End Of Request Pipe
      close(response_pipes[i][0]); // Close Read End Of Response Pipe
    }
  }

  int total_requests = 0; // Total Number Of Requests
  int successful_bookings = 0; // Number Of Successful Bookings
  int total_profits = 0; // Total Profits Of The Cinema
  int total_tickets_sold = 0; // Total Tickets Sold
  // Process Requests From Clients
  for (int i = 0; i < MAX_CLIENTS; i++) {
    // Wait For Request From Client
    char request[100];
    if (read(request_pipes[i][0], request, 100) == -1) {// If The 'read' Function Is Successful, It Will Return The Number Of Characters Read
                                                        // And The Data Will Be Stored In The 'request' Array.
      perror("Error Reading From Request Pipe");
      return 1;
    } else {
      total_requests++;
    }

    // Parse Request
    int num_tickets;
    int movie_id;
    sscanf(request, "%d %d", &num_tickets, &movie_id);

    // Simulate Processing Time
    usleep(SERVICE_TIME);

    // Check If Request Can Be Fulfilled
    if (movies[movie_id].available_seats >= num_tickets) {

      movies[movie_id].available_seats -= num_tickets; // Update Number Of Available Seats
      total_tickets_sold += num_tickets; // Counting Total Tickets
      int cost = num_tickets * movies[movie_id].price; // Calculating Cost Per Movie
      movies[movie_id].successful++; // Counting Successful Booking Per Movie

      // Send Response To Client
      char response[] = "Success"; // If There Are Available Seats Then Response = "Success"
      successful_bookings++; // Counting Successful Bookings
      total_profits += cost; // Calculating Total Costs
      
      if (write(response_pipes[i][1], response, strlen(response) + 1) == -1) { // 1st Argument = Is The File Descriptor To Which The Data Should Be Written.
                                                                               // 2nd Argument = Is A Pointer To The Data That Should Be Written.
                                                                               // 3rd Argument = Is The Length Of The Data To Be Written. (+1 To Include The Null Terminator)
        perror("Error Writing To Response Pipe");
        return 1;
      }
    } else {
      
      movies[movie_id].unsuccessful++; // Increment Unsuccessful Bookings

      // Send Response To Client
      char response[] = "Failed"; // If There Are No Available Seats Server Sents To Client A Response = "Failed"
      if (write(response_pipes[i][1], response, strlen(response) + 1) == -1) {
        perror("Error Writing To Response Pipe");
        return 1;
      }
    }
  }

  // Wait For Child Processes To Finish
  for (int i = 0; i < MAX_CLIENTS; i++) {
    wait(NULL);
  }

  // Print Updated Movies Table (After Deducting The Tickets That Have Been Sold)
  printf("\n\t===== Update Movies Table =====\n");
  print_movies(movies);

  printf("\n\t=========== Results ===========\n");
  // Print Movie ID, Total Requests, Number Of Successful And Unsuccessful Bookings For Each Movie
  printf("Movie\tRequests\tSuccessful\tUnsuccessful\n");
  for (int i = 0; i < MAX_MOVIES; i++) {
    printf("%d\t%d\t\t%d\t\t%d\n",i,movies[i].successful+movies[i].unsuccessful, movies[i].successful, movies[i].unsuccessful);
  }

  printf("\n\t=========== Summary ===========\n");
  // Print Total Requests, Successful Bookings, Profits & Tickets
  printf("Total Requests: %d\n", total_requests);
  printf("Total Successful Bookings: %d\n", successful_bookings);
  printf("Total Profits: %d\n", total_profits);
  printf("Total Tickets Sold: %d\n\n", total_tickets_sold);

  return 0;
}
