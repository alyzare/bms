import asyncio
import websockets

async def hello():
    uri = "ws://localhost:8080"  # Replace with your WebSocket server URI
    async with websockets.connect(uri) as websocket:
        await websocket.send("\r\n\r\n0101")
        print("Sent: hello")

asyncio.get_event_loop().run_until_complete(hello())