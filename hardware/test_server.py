import asyncio
import websockets
import signal

stop_event = asyncio.Event()
flag = True
# Function to handle received messages
async def receive_messages(websocket):
    async for message in websocket:
        print(f"Received: {message}")

# Function to send messages every second
async def send_messages(websocket):
    global flag
    while not stop_event.is_set():
        if flag:
            await websocket.send("\r\n\r\n0101")
        else:
            await websocket.send("\r\n\r\n1010")
        print("Sent order")
        flag = not flag
        await asyncio.sleep(1)

# Function to connect to WebSocket server and handle communication
async def connect_to_websocket():
    uri = "ws://172.16.0.3:81"  # Replace with your WebSocket server URL
    async with websockets.connect(uri) as websocket:
        receive_task = asyncio.create_task(receive_messages(websocket))
        send_task = asyncio.create_task(send_messages(websocket))

        await asyncio.gather(receive_task, send_task)

# Signal handler to stop the event loop
def signal_handler():
    stop_event.set()

# Run WebSocket connection in an asyncio event loop
async def main():
    # Register signal handlers for graceful shutdown
    loop = asyncio.get_running_loop()
    try:
        loop.add_signal_handler(signal.SIGINT, signal_handler)
        loop.add_signal_handler(signal.SIGTERM, signal_handler)
    except NotImplementedError:
        # Windows does not implement add_signal_handler
        import threading
        def windows_signal_handler():
            signal_handler()
        signal.signal(signal.SIGINT, lambda s, f: threading.Thread(target=windows_signal_handler).start())
        signal.signal(signal.SIGTERM, lambda s, f: threading.Thread(target=windows_signal_handler).start())

    try:
        await connect_to_websocket()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        print("WebSocket connection closed.")

if __name__ == "__main__":
    asyncio.run(main())