#!/bin/bash

echo "Testing signal handling..."
echo "This will run 'sleep 10' in minishell"
echo "The signal handling should work correctly now"

# minishell'i çalıştır ve sleep komutunu ver
echo "sleep 10" | ./minishell

echo "Test completed" 