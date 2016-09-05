#!/bin/bash
username=`zenity --entry --title "USER NAME?" --text="NAME"`
password=`zenity --entry --title "PASSWORD?" --text="NAME" --hide-text`
mysql -u $username --password=$password < load.sql
