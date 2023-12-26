import requests
import json

def send_slack_message(webhook_url, message, color):
    payload = {
        "mrkdwn": True,
        "attachments": [
            {
                "color": color,
                "text": message
            }
        ]
    }

    headers = {
        "Content-Type": "application/json",
    }

    response = requests.post(webhook_url, data=json.dumps(payload), headers=headers)

    if response.status_code == 200:
        # print("Message sent successfully!")
        print(" ")
    else:
        print(f"Failed to send message. Status code: {response.status_code}, Response: {response.text}")


