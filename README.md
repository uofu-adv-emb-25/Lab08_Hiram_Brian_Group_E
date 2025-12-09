## Activity 1

#### Images of a message being sent and recieved through the CAN bus.

![Serial_Message](./images/activity1_tx.png)

![Serial_Message](./images/activity1_rx.png)


#### Broadcasting a babble noise with high priority and an important message with low priority.

##### Utilizing a short delay of 10 ms, we got a small amount of the important message to send.

![Serial_Message](./images/10msDelayCANH.PNG)

![Serial_Message](./images/10msDelayCANL.PNG)

##### When the delay was increased to 100 ms, we could further see more important messages making it through the babbling noise.

![Serial_Message](./images/100msDelayCANH_and_L.PNG)

##### As we got up to a 1 second delay on the babbling noise, we were able to completely get an even breakthrough of noise and important messages.

![Serial_Message](./images/1000msDelayCANH_and_L.PNG)
