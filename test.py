import pxnd
import pyarrow.plasma as plasma

client = plasma.connect("/tmp/plasma", "", 0)

object_id = plasma.ObjectID.from_random()
print(object_id)
pxnd.put(client, object_id, 23)
