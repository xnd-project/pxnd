# `pxnd`

**WIP**: This is being actively developed and doesn't work yet.

Parallel/Plasma xnd.

## Usage
First start a plasma store:

```bash
plasma_store -m 1000000000 -s /tmp/plasma
```

Then, connect to it and create some data in it:

```python
>>> import pyarrow.plasma as plasma
>>> import pxnd
>>> client = plasma.connect("/tmp/plasma", "", 0)
>>> object_id = plasma.ObjectID.from_random()
>>> object_id
ObjectID(6161616161616161616161616161616161616161)
>>> x = pxnd.put(client, object_id, [[0, 1, 2], [3, 4, 5]])
xnd([[0, 1, 2], [3, 4, 5]], type="2 * 3 * int64")
```

In another process, you can then also read the data:

```python
>>> import pyarrow.plasma as plasma
>>> import pxnd
>>> import bytes
>>> client = plasma.connect("/tmp/plasma", "", 0)
>>> object_id = plasma.ObjectID(bytes.fromhex('6161616161616161616161616161616161616161'))
>>> pxnd.get(client, object_id)
xnd([[0, 1, 2], [3, 4, 5]], type="2 * 3 * int64")
```

## Dev setup

```bash
pip install flit
make
cd python
flit install --symlink
```

