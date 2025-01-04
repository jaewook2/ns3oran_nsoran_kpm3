# ns3oran_nsoran_kpm3
 Forked from nsoran of ORANGE (orange version) [https://github.com/MinaYonan123/oran-interface/tree/master]

 
## Main Updates

### The implemetation of the E2APv1.01 includes:

 1. **E2 Setup request**
 
        - Edit the RAN Function IDs for KPM and RC ​
        - Reformat the size of the message to match FlexRIC (62 byte)​
        - Update the KPM description and model to KPM v3 
        - Upda​te the RC  description and model to RC v1.03
        - Add RIC styles 4 
 
 2. **E2 Subscription Response** ​
 
        - Add RAN Function NotAdmitted IE to the message​
        - Update the message to match KPM v3.0​
        - Fill New RIC indication messages formats (Format 3 )​
 
3. **RIC Control Acknoweldege** ​

          - Implemented from scratch
4. **RIC Control Request** ​

          - Update the message to match E2SM RC v1.03
 
 5. **RIC Subscription delete request/response**
      
      
### The implementation of KPM and RC

1. Update the KPM ASN and model to KPM v3.00

2. Update the RC ASN and model to RC v1.03
