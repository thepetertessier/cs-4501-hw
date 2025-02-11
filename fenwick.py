DEBUG = False

def debug(msg):
    if DEBUG:
        print(msg)

class OneIndexedList:
    def __init__(self, iterable=None):
        """Initialize the list with an optional iterable."""
        self._data = list(iterable) if iterable else []

    def __getitem__(self, index):
        """Get an item using 1-based indexing."""
        if index < 1 or index > len(self._data):
            raise IndexError("Index out of range")
        return self._data[index - 1]

    def __setitem__(self, index, value):
        """Set an item using 1-based indexing."""
        if index < 1 or index > len(self._data):
            raise IndexError("Index out of range")
        self._data[index - 1] = value

    def __iter__(self):
        """Allow iteration over the list."""
        return iter(self._data)

    def __len__(self):
        """Return the number of elements in the list."""
        return len(self._data)

    def append(self, value):
        """Append a new element to the list."""
        self._data.append(value)

    def __repr__(self):
        return f"OneIndexedList({self._data})"

class Fenwick:
    def __init__(self, servers, capacity):
        self.capacity = capacity
        self.t = OneIndexedList([0]*len(servers))
        self.a = OneIndexedList(servers)
        for i, server in enumerate(servers, start=1):
            self.t[i] += server
            debug(f'\nt[{i}] = {self.t[i]-server} + {server} = {self.t[i]}')
            j = self.reverse(i)
            if j <= len(self.t):
                self.t[j] += self.t[i]
                debug(f't[{j}] = {self.t[j]-self.t[i]} + {self.t[i]} = {self.t[j]}')

    def store_data(self, size, index):
        to_store = size
        for i in range(index, min(index+5, len(self.t)+1)):
            if to_store <= 0:
                return
            stored_at_i = self.a[i]
            if to_store + stored_at_i > self.capacity:
                # There's more to store than t[i] can hold - fill it and move on
                to_store_here = self.capacity - stored_at_i
                self.update(i, to_store_here)
                to_store -= to_store_here
            else:
                # Store the rest - we can take it
                self.update(i, to_store)
                return

    def _find_deletion_index(self, t):
        # sum_before_idx is always a[1] + ... + a[idx-1]
        # idx is always the minimum possible that satisfies given info
        # mask goes 2^n -> 2^(n-1) -> ... -> 2^0
        sum_before_idx = 0
        idx = 1
        mask = 1 << (len(self.t).bit_length() - 1)
        while mask > 0:
            i = mask + idx - 1
            sum_at_i = self.t[i] + sum_before_idx if i <= len(self.t) else float('inf')
            if sum_at_i < t:
                # Go right
                idx = i + 1
                sum_before_idx = sum_at_i
            mask >>= 1
        return idx

    def delete_data(self, size, threshold):
        # 1. Find minimum i in [0..n-1] s.t. sum(i) >= t (threshold)
        # 2. Delete up to size data from t[i]
        index = self._find_deletion_index(threshold)
        stored_at_index = self.a[index]
        to_delete = min(stored_at_index, size)
        self.update(index, -to_delete)

    def query_data(self, left: int, right: int):
        '''both inclusive'''
        if right - left + 1 < right.bit_count() + (left-1).bit_count():
            # This will be faster
            return sum(self.a[i] for i in range(left, right+1))
        else:
            return self.sum(right) - self.sum(left-1)
    
    def sum(self, index):
        '''sums from indices 1 to index INCLUSIVE'''
        if index < 1:
            return 0
        if index > len(self.t):
            raise ValueError(f'Tried to get sum from 1 to {index}, which is higher than the max ({len(self.t)})')
        return self.t[index] + self.sum(index & (index-1))
    
    def reverse(self, index):
        # Add the least significant digit that's set to 1
        return index + (index & -index)
    
    def update(self, index, delta, first=True):
        debug(f'Now updating index {index} with delta {delta}')
        if index < 1 or index > len(self.t):
            return
        if first:
            self.a[index] += delta
        self.t[index] += delta
        self.update(self.reverse(index), delta, first=False)

    def __str__(self):
        return '\n'.join(f'{i}: {val}' for i, val in enumerate(self.t, start=1))

def test():
    # servers = OneIndexedList([3,7,1,4,5,4,3,1,1,2])
    servers = range(1,12)
    fenwick = Fenwick(servers, 20)
    print(fenwick)

    for i in range(1, len(servers)+1):
        print(f'sum from 1 to {i}: {fenwick.sum(i)}')

def main():
    server_count, capacity, command_count = [int(n) for n in input().strip().split()]
    servers = [int(n) for n in input().strip().split()]
    fenwick = Fenwick(servers, capacity)
    for _ in range(command_count):
        command, x, y = input().strip().split()
        x, y = int(x), int(y)
        match command:
            case 'F':
                fenwick.store_data(x, y+1)
            case 'D':
                fenwick.delete_data(x, y)
            case 'Q':
                size = fenwick.query_data(x+1, y+1)
                print(f'({x},{y}): {size}')
            case _:
                raise RuntimeError(f'Command not recognized: {command}')

if __name__ == '__main__':
    main()
