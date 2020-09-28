#!/usr/bin/env python
# vim: set rnu sw=4 ss=4 ts=4 et smartindent autoindent fdm=indent :
# -*- coding: utf-8 -*-
# This program is dedicated to the public domain under the CC0 license.

"""
Basic example for a bot that uses inline keyboards.
"""
from MyToken import TOKEN
import logging
import requests
from time import sleep
import threading

from telegram import InlineKeyboardButton, InlineKeyboardMarkup
from telegram.ext import Updater, CommandHandler, CallbackQueryHandler

logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                    level=logging.INFO)
logger = logging.getLogger(__name__)

keyboard = [
    [InlineKeyboardButton("상태 확인", callback_data=0), InlineKeyboardButton("바로 끄기", callback_data=-1)],
    [InlineKeyboardButton("1시간 켜기", callback_data=1), InlineKeyboardButton("2시간 켜기", callback_data=2)],
    [InlineKeyboardButton("3시간 켜기", callback_data=3), InlineKeyboardButton("4시간 켜기", callback_data=4)],
    [InlineKeyboardButton("5시간 켜기", callback_data=5), InlineKeyboardButton("6시간 켜기", callback_data=6)],
    [InlineKeyboardButton("7시간 켜기", callback_data=7), InlineKeyboardButton("8시간 켜기", callback_data=8)],
    [InlineKeyboardButton("9시간 켜기", callback_data=9), InlineKeyboardButton("10시간 켜기", callback_data=10)],
    [InlineKeyboardButton("11시간 켜기", callback_data=11), InlineKeyboardButton("12시간 켜기", callback_data=12)]
]

reply_markup = InlineKeyboardMarkup(keyboard)


BULB_URL = "http://192.168.0.44"

def start(update, context):
    update.message.reply_text("알프레드가 시작되었습니다.", reply_markup=reply_markup)


def button(update, context):
    query = update.callback_query

    # CallbackQueries need to be answered, even if no notification to the user is needed
    # Some clients may have trouble otherwise. See https://core.telegram.org/bots/api#callbackquery
    query.answer()
    seconds = int(query.data) * 3600
    if(0 < seconds):
        rq = requests.get(BULB_URL + '/led', params={'Params':seconds})
        hleft = round(rq.json()["return_value"] / 3600, 2) # sec to h and round
        query.edit_message_text(text="🌕 켰습니다. {}시간 남음.".format(hleft), reply_markup=reply_markup)
    elif(0 > seconds):
        rq = requests.get(BULB_URL + '/led', params={'Params':seconds})
        query.edit_message_text(text="🌚 껐습니다.", reply_markup=reply_markup)
    else:
        rq = requests.get(BULB_URL + '/status')
        hleft = round(rq.json()["return_value"] / 3600, 2) # sec to h and round
        if(0 < hleft):
            query.edit_message_text(text="🌕 켜져 있습니다. {}시간 남음.".format(hleft), reply_markup=reply_markup)
        else:
            query.edit_message_text(text="🌚 꺼져 있습니다. ", reply_markup=reply_markup)


def help_command(update, context):
    update.message.reply_text("알프레드가 시작되었습니다.", reply_markup=reply_markup)



def main():
    bulbtoken = TOKEN
    bulb_updater = Updater(bulbtoken, use_context=True)

    bulb_updater.dispatcher.add_handler(CommandHandler('start', start))
    bulb_updater.dispatcher.add_handler(CallbackQueryHandler(button))
    bulb_updater.dispatcher.add_handler(CommandHandler('help', help_command))

    # Start the Bot
    bulb_updater.start_polling()

    # Run the bot until the user presses Ctrl-C or the process receives SIGINT,
    # SIGTERM or SIGABRT
    bulb_updater.idle()

if __name__ == '__main__':
    main()

