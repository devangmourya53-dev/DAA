# Demonstration script
1. Build: `make` and verify: `make test`.
2. Run menu option 4: normal small scenario, 6 locations, 6 orders, capacity 12.
3. Run option 5: same orders with capacity reduced to 5; compare greedy and exact knapsack value.
4. Run option 6: 120 locations, 700 orders, capacity 500; inspect runtime and memory estimates.
5. Run option 7: adversarial medium case with many equal-priority orders.
6. Run option 8: print all benchmark reports.
7. Explain that route algorithms agree on distances; greedy may be worse than knapsack, while knapsack uses more time as capacity grows. Mention that no end-to-end multi-stop tour is computed.
