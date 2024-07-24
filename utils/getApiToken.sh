#!/bin/bash


CLIENT_ID=u-s4t2ud-3508d76eed7e54c9bae8c7409b1d9dd376601e9f05adf235bf229362f6bc3810
CLIENT_SECRET=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

curl -X POST --data "grant_type=client_credentials&client_id=$CLIENT_ID&client_secret=$CLIENT_SECRET" https://api.intra.42.fr/oauth/token > utils/api_token.txt
