import pxnd
import pyarrow.plasma as plasma
import xnd

client = plasma.connect("/tmp/plasma", "", 0)

object_id = plasma.ObjectID.from_random()
print(client.put("dfd"))
print(object_id)
pxnd.put(client, object_id, xnd.typeof(23), 23)
