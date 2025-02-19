import asyncio
import websockets
import sqlite3
import json
from datetime import datetime

ip_address = ""

class BackendServer:
    def __init__(self, udp_port=1380,
                 ws_server_host="127.0.0.1", ws_server_port=82,
                 ws_client_url="ws://localhost:9000"):
        # Configuration
        self.udp_port = udp_port
        self.ws_server_host = ws_server_host
        self.ws_server_port = ws_server_port
        self.ws_client_url = ws_client_url
        
        # Initialize database
        self.init_database()
        
        # Store connected WebSocket clients
        self.clients = set()

    def init_database(self):
        """Initialize SQLite database and create necessary tables"""
        self.conn = sqlite3.connect('backend.db')
        cursor = self.conn.cursor()
        
        # Create a table to store sensor values (temp,hum,gas) and timestamp as pk
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS sensor_values (
                timestamp TEXT PRIMARY KEY,
                temperature REAL,
                humidity REAL,
                gas REAL);
        ''')
        
        # create a table to store scenarios and their corresponding actions
        # scenarioSensor is the sensor value that triggers the action
        # action is an integer that corresponds to the action to be taken
        # conditionDown is the lower bound of the sensor value
        # conditionUp is the upper bound of the sensor value
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS scenarios (
                id INTEGER PRIMARY KEY,
                scenarioSensor TEXT,
                action INT,
                conditionDown REAL,
                conditionUp REAL
                );
        ''')
        
        # create a table that store the summerization of past data
        # timestamp is the pk
        cursor.execute(
            '''
            CREATE TABLE IF NOT EXISTS summary (
                timestamp TEXT PRIMARY KEY,
                temperature REAL,
                humidity REAL,
                gas REAL
            );
            '''
        )
        
        self.conn.commit()
        
        async def summarize_and_cleanup(self):
            """Summarize data older than 5 minutes and clean up"""
            while True:
                cursor = self.conn.cursor()
                # Get current time
                now = datetime.utcnow()
                # Calculate the time 5 minutes ago
                five_minutes_ago = now - timedelta(minutes=5)
                five_minutes_ago_str = five_minutes_ago.strftime('%Y-%m-%d %H:%M:%S')

                # Select data older than 5 minutes
                cursor.execute('''
                    SELECT * FROM sensor_values WHERE timestamp < ?
                ''', (five_minutes_ago_str,))
                rows = cursor.fetchall()

                if rows:
                    # Summarize data
                    avg_temp = sum(row[1] for row in rows) / len(rows)
                    avg_humidity = sum(row[2] for row in rows) / len(rows)
                    avg_gas = sum(row[3] for row in rows) / len(rows)

                    # Insert summarized data into summary table
                    cursor.execute('''
                        INSERT INTO summary (timestamp, temperature, humidity, gas)
                        VALUES (?, ?, ?, ?)
                    ''', (five_minutes_ago_str, avg_temp, avg_humidity, avg_gas))

                    # Delete old data
                    cursor.execute('''
                        DELETE FROM sensor_values WHERE timestamp < ?
                    ''', (five_minutes_ago_str,))

                    self.conn.commit()

                # Wait for 1 minute before running again
                await asyncio.sleep(60)
                
    async def handle_udp(self):
        """Handle incoming UDP messages"""
        # Create UDP endpoint
        transport, protocol = await asyncio.get_event_loop().create_datagram_endpoint(
            lambda: UDPProtocol(self),
            local_addr=("0.0.0.0", self.udp_port)
        )

        try:
            while True:
                await asyncio.sleep(1)
        except asyncio.CancelledError:
            transport.close()

    async def handle_ws_client(self):
        """Connect to external WebSocket server as a client"""
        while True:
            try:
                async with websockets.connect(self.ws_client_url) as websocket:
                    print(f"Connected to WebSocket server at {self.ws_client_url}")
                    while True:
                        message = await websocket.recv()

                        await self.handle_message("ws_client", message)
            except websockets.ConnectionClosed:
                print("Connection to WebSocket server lost. Reconnecting...")
                await asyncio.sleep(5)
            except Exception as e:
                print(f"WebSocket client error: {e}")
                await asyncio.sleep(5)

    async def handle_ws_connection(self, websocket, path):
        """Handle incoming WebSocket connections"""
        self.clients.add(websocket)
        try:
            async for message in websocket:
                await self.handle_message("ws_server", message)
                # Broadcast message to all connected clients
                await self.broadcast_message(message)
        finally:
            self.clients.remove(websocket)

    async def broadcast_message(self, message):
        """Broadcast message to all connected WebSocket clients"""
        if self.clients:
            await asyncio.gather(
                *[client.send(message) for client in self.clients]
            )

    async def handle_message(self, source, message):
        """Process and store incoming messages"""
        # Store message in database
        cursor = self.conn.cursor()
        cursor.execute(
            "INSERT INTO messages (source, content) VALUES (?, ?)",
            (source, message)
        )
        self.conn.commit()
        
        # Print message for debugging
        print(f"Received from {source}: {message}")

    async def run(self):
        """Start all server components"""
        # Start WebSocket server
        ws_server = await websockets.serve(
            self.handle_ws_connection,
            self.ws_server_host,
            self.ws_server_port
        )
        
        # Create tasks for UDP handler and WebSocket client
        udp_task = asyncio.create_task(self.handle_udp())
        ws_client_task = asyncio.create_task(self.handle_ws_client())
        
        print(f"UDP Server listening on 0.0.0.0:{self.udp_port}")
        print(f"WebSocket Server running on ws://{self.ws_server_host}:{self.ws_server_port}")
        print(f"WebSocket Client connecting to {self.ws_client_url}")
        
        # Keep the server running
        await asyncio.gather(udp_task, ws_client_task)

class UDPProtocol(asyncio.DatagramProtocol):
    def __init__(self, server):
        self.server = server

    def datagram_received(self, data, addr):
        """Handle incoming UDP messages"""
        message = data.decode()
        asyncio.create_task(
            self.server.handle_message("udp", message)
        )
        print(f"Received UDP message from {addr}: {message}")
        if message == "BMS_HARDWARE":
            global ip_address
            ip_address = addr[0]
            asyncio.create_task(
                self.server.connect_to_ip_address(ip_address, 81)
            )


# Run the server
if __name__ == "__main__":
    server = BackendServer()
    asyncio.run(server.run())