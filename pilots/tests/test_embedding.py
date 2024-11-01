from transformers import AutoTokenizer, AutoModel
import torch

# Load the BERT model and tokenizer
tokenizer = AutoTokenizer.from_pretrained("bert-base-uncased")
model = AutoModel.from_pretrained("bert-base-uncased")

# Tokenize the input text
input_text = "Hello, my dog is cute"
input_ids = tokenizer(input_text, return_tensors="pt")["input_ids"]

# Get the embeddings
with torch.no_grad():
    output = model(input_ids=input_ids)

# The output is a tuple with the last element being the hidden states
hidden_states = output.last_hidden_state
print(hidden_states.shape)
