# Interval Tree Based Event Scheduler

## Overview
This project implements an **Event Scheduling System** using an **Interval Tree with AVL balancing**. The system can:
- Add events with time intervals.
- Detect and prevent scheduling conflicts.
- Search and explain events at a given time.
- Schedule event notifications.
- Delete events by ID.
- Display all scheduled events.

The system supports **real-time interval management** with efficient insertion, deletion, and querying operations.

---

## Features
- ✅ Insert events with descriptions and time intervals.
- ✅ Automatically checks for overlapping (conflicting) events.
- ✅ Supports deletion of events by their unique IDs.
- ✅ In-order traversal display for event overview.
- ✅ Schedule notifications for events (with time, method, and recipient).
- ✅ Query and display all events occurring at a specific time.
- ✅ Efficient AVL-based balancing ensures fast operations.
- ✅ Memory cleanup upon exit.

---

## Data Structures Used
- **Interval Tree (AVL Balanced):** For managing time intervals efficiently.
- **Unordered Map:** Maps event IDs to interval start times for quick lookup.

---

## How It Works
1. **Insert Event:** Add an event with a description and a time interval. Conflicting events are not allowed.
2. **Explain Events at Time:** Query all events happening at a specific time and display associated notifications.
3. **Delete Event:** Remove an event by providing its unique ID.
4. **Display All Events:** Show all scheduled events in in-order traversal.
5. **Schedule Notification:** Set a notification time, method (like email, SMS), and recipient for a specific event.
6. **Exit:** Properly deallocates memory and closes the program.

---

## Code Structure
- `Interval` - Structure to store event details.
- `ITNode` - Node structure for the Interval Tree.
- AVL rotations (`leftRotate`, `rightRotate`) for balancing.
- Functions for:
  - Insertion with conflict detection
  - Deletion by ID
  - Event search
  - Notification scheduling and checking
  - Querying events at a specific time
- User-driven menu-based interface in `main()`.

---

## Requirements
- **Language:** C++
- **Compiler:** Any standard C++ compiler supporting C++11 or later (tested on g++).

---
