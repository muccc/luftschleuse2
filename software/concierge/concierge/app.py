from fastapi import FastAPI, Form
from fastapi.responses import HTMLResponse, RedirectResponse
import uvicorn
import requests
import socket
import os
from anyio import create_udp_socket
from typing import Optional

BELL_CODE = os.getenv('CONCIERGE_BELL_CODE', 'test')

app = FastAPI()

@app.get('/')
async def root():
    try:
        state = requests.get("http://api.muc.ccc.de/schleuse.json").json()['state']
    except:
        state = 'unknown'

    return HTMLResponse('''
<!DOCTYPE html>
<html>
    <head>
        <title>Luftschleuse Concierge</title>
    </head>
    <body>
        <h2>current state: {}</h2>

        <h3>change hq state</h3>
        <ul style="list-style: none">
            <li><button onclick="window.location.href='/down'">down</button></li>
            <li><button onclick="window.location.href='/closed'">closed</button></li>
        </ul>

        <h3>open door if member</h3>
        <ul style="list-style: none">
            <form action="/dooropen" method="POST">
            <li>Bell Code: <input name="bellcode" type="password" /></li>
            <li><button name="door" value="back">Open Back Door</button></li>
            <li><button name="door" value="front">Open Front Door</button></li>
            </form>
        </ul>
    </body>
</html>
    '''.format(state))


@app.get('/closed')
async def down():
    await lockd_send_command('closed')
    return RedirectResponse('/', status_code=302)


@app.get('/down')
async def down():
    await lockd_send_command('down')
    return RedirectResponse('/', status_code=302)


@app.post('/dooropen')
async def dooropen(bellcode: str = Form(),
                   door: Optional[str] = Form()):
    if bellcode == BELL_CODE:
        cmd = None
        if door == 'back':
            cmd = 'bell_code_back'
        elif door == 'front':
            cmd = 'bell_code_front'
        await lockd_send_command(cmd)
        return HTMLResponse('ok: {}'.format(cmd))
    return HTMLResponse('wrong')


async def lockd_send_command(command, host='127.0.0.1', port=2323):
    async with await create_udp_socket(family=socket.AF_INET) as udp:
        await udp.sendto(command.encode('utf-8'), host, port)


def run():
    server = uvicorn.Server(uvicorn.Config(app, host='::', port=8010))
    server.run()
