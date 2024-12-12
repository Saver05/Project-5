# Project 5: Concurrency

## Part 1: The Dining Philosophers

### Algorithm #1
> In algorithm 1 both deadlock and starvation occur. 
> If every philosopher picks up their left fork at the exact same time
> Deadlock will occur locking the system in place as no one can pick up
> their right fork to finish the eating process.
> Starvation occurs if one philosopher consistently grabs the fork on their
> right before the philosopher on their left can grab it. While it will be 
> put down eventually it will still block the philosopher on the right from
> picking up the fork for a bit

### Algorithm #2
> In algorithm 2 starvation occurs and deadlock is very unlikely
>  Deadlock is reduced as their is careful checking of fork availability
> however it can still occur in rare scenarios
> Starvation however can occur as if the same two philosophers on either side of 
> another one pick their forks up alternating the middle philosopher will never be
> able to grab theirs
> 

### Algorithm #3
> Allowing N-1 philosophers prevents deadlock and starvation is very unlikely
> 
## Part 2: Barber Shot

### Algorithm 
> Currently working for 2 barbers outside of the fact they for some reason never 
> move on to the next customer