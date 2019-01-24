# Test Cases

## Test 0

Tests basic sending/receiving.

Very simple test case: Send a single packet through the network.

## Test 1

Tests that multiple VCs are used.

Test case: Send two packets at the same time from routers 0 and 2. These meet
at router 1 at the same time, one coming from WEST, the other from EAST. Both
will go the NORTH. Therefore, one must go to VC0, the other to VC1. The sending
must be fair, i.e. VC0 and 1 must be interleaved.