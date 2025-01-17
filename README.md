## Summary

This project implements a **Distributed Hash Table (DHT)** that combines efficient data indexing and retrieval with dynamic node management in a scalable distributed system. The core design simulates a **ring network of machines**, where each machine is responsible for storing and retrieving data based on hashed identifiers. A variety of **data structures** and **algorithms** are employed to ensure the system's efficiency and modularity:

- **B-Trees**: Used on each machine to index and retrieve files efficiently.
- **Circular Singly Linked List**: Represents the ring topology of machines, where each node connects to its successor.
- **Doubly Linked List**: Implements routing tables, enabling shortcut navigation across non-adjacent machines for O(log N) lookups.
- **Hashing**: Maps file keys to specific machines in the ring, ensuring uniform data distribution.

The project also supports dynamic operations such as adding or removing nodes, redistributing data when required, and handling file operations (insertion, deletion, and search). Using advanced routing mechanisms, it reduces file lookup complexity from **O(N)** to **O(log N)**, making it a robust and efficient solution for distributed systems handling large-scale datasets.

## File Storage and Retrieval

### Data Storage
- Files are stored on machines using a B-tree structure for indexing:
  - **Keys**: Hash of the file content (e.g., `SHA-1("wizard_of_oz")`).
  - **Values**: Path to the actual file stored on the hard disk.
- Files are stored in the ring network based on their key (`e`):
  - The file is placed on the machine `p` such that `p ≥ e`.
  - If no machine satisfies this, it wraps around the circular identifier space.

### File Retrieval
- To retrieve a file:
  1. Find the machine responsible for the file key.
     - Search starts on the current machine and forwards the query as necessary using the circular linked list or the routing table.
  2. Once the machine is identified, use the B-tree to locate the file.

## Ring DHT Functionality

### Node Structure
- The network forms a **circular singly linked list** of machines (nodes).
- Each node maintains:
  - **B-tree**: For indexing and storing files (key, value pairs).
  - **Routing Table**: Implements shortcuts for efficient file lookups.

### Routing Table
- Each machine `p` maintains a routing table `FTp` with up to `O(log N)` entries:
  - Each entry provides a shortcut to distant nodes using the formula `FTp[i] = succ(p + 2^(i-1))` where `i = 1, ..., log(N)`.
  - Example: For machine with `id=1` in a 5-bit identifier space, the table includes successors at distances 1, 2, 4, 8, 16.

### Search Algorithm
1. **Case 1**: If the machine `p` is responsible for the key (`p ≥ e`), perform a B-tree search to retrieve the file.
2. **Case 2**: If `p < e`, forward the request:
   - If `FTp[1] ≥ e`, forward the query to `FTp[1]`.
   - Else, find the appropriate interval `FTp[j] < e ≤ FTp[j+1]` and forward to `FTp[j]`.
3. Use modulo operation for circular identifier space.

### Complexity
- Without routing table: `O(N)` lookups.
- With routing table: `O(log N)` lookups due to efficient shortcuts.

## Dynamic Node Management

### Adding a Node
- Compute the hash for the new machine.
- Update its routing table and adjust the routing tables of other machines.
- Redistribute identifier ranges and move corresponding files.
- Update B-trees of affected machines.

### Removing a Node
- Redistribute files from the departing node to its neighbors.
- Update routing tables and B-trees accordingly.

## Commands and Features
The system supports the following:
1. **Specify Network Configuration**:
   - Set the number of machines in the system.
   - Set the identifier space size (e.g., 4-bit, 160-bit).
   - Manually assign IDs or allow the system to auto-assign.
2. **Insert Files**:
   - Add files from any machine.
   - Display the full path taken by the request.
   - Option to print the machine's B-tree.
3. **Remove Files**:
   - Remove files by key from any machine.
   - Display the full path taken by the request.
   - Print the updated B-tree.
4. **Print Routing Table**:
   - Display the routing table of any machine.
5. **Add/Remove Machines**:
   - Dynamically add new machines without disrupting functionality.
   - Gracefully remove machines, redistributing files as necessary.

## Simplifications for Implementation
- Emulated on a single machine for demonstration purposes.
- Uses text files with minimal content instead of large files like videos.

