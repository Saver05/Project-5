# Project 5: Concurrency

## Part 1: The Dining Philosophers

### Algorithm #1
**DiningPhilosophers1.c**
> In algorithm 1 both deadlock and starvation occur. 
> If every philosopher picks up their left fork at the exact same time
> Deadlock will occur locking the system in place as no one can pick up
> their right fork to finish the eating process.
> Starvation occurs if one philosopher consistently grabs the fork on their
> right before the philosopher on their left can grab it. While it will be 
> put down eventually it will still block the philosopher on the right from
> picking up the fork for a bit

### Algorithm #2
**DiningPhilosophers2.c**
> 