
# Cinema Ticket Booking System

This is a simple cinema ticket booking system implemented in C. It simulates a cinema where clients can request tickets for movies, and the server processes the requests and responds accordingly.

## Features

- Supports a maximum of 10 clients and 10 movies.
- Each movie has a start and end time, available seats, and a ticket price.
- The server receives requests from clients and checks if the requested number of tickets is available for the specified movie.
- If the tickets are available, the server processes the booking, deducts the number of tickets from the available seats, and calculates the cost.
- If the tickets are not available, the server responds with a "Failed" message.
- The system keeps track of successful and unsuccessful booking requests for each movie.
- At the end, the system prints the updated movies table, along with statistics such as total requests, successful bookings, profits, and total tickets sold.

## Usage

To compile and run the program, use the following commands:

```shell
gcc -o cinema_booking cinema_booking.c
./cinema_booking
```

The program will display the movies table, simulate client requests, process the requests, and provide the final results.

## Customization

You can customize the system by modifying the following constants defined in the code:

- `MAX_CLIENTS`: Maximum number of clients (default: 10).
- `MAX_MOVIES`: Maximum number of movies (default: 10).
- `MAX_SEATS`: Maximum number of seats per movie (default: 5).
- `SERVICE_TIME`: Time to process a booking request in microseconds (default: 500,000, equivalent to 0.5 seconds).

Feel free to modify these values according to your requirements.
