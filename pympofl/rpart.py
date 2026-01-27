import redis
app = redis.Redis(host="localhost", port=1488)
app.set("nord", "rаб".encode("utf-16"))
print(bytes(app.get("nord")).decode("utf-16"))