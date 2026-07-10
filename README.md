# Retail Inventory & Billing Suite

A modular, terminal-based enterprise stock management and point-of-sale (POS) billing engine built in C++. This project highlights low-level system design patterns and object-oriented architectures.

## 🛠️ Core Engineering Highlights
* **Encapsulation & Abstraction:** Securely wraps product parameters (IDs, strict pricing margins, and real-time inventory balances) using access modifiers.
* **Inheritance models:** Establishes a foundational base `Item` blueprint dynamically extended by specialized derived subclasses (`PerishableItem` and `ElectronicsItem`).
* **Runtime Polymorphism:** Implements virtual function overrides via a uniform pointer tracking system (`vector<Item*>`) to determine class behaviors dynamically during active execution loops.
* **Flat-File Database Persistence:** Utilizes custom C++ file stream processing (`ifstream`/`ofstream`) to parse, update, and persist system state transformations to a local storage file cache.

## 🚀 Execution & Setup Guide
To run this application locally, ensure you have a standard C++ compiler installed (like GCC/MinGW).

1. Clone or download this project workspace.
2. Compile the core script engine using a terminal prompt:
   ```bash
   g++ main.cpp -o InventorySuite
