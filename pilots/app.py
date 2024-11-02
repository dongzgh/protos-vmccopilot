from utils import search_text
from os.path import realpath, join

# Ask user for input.
query = input ("Enter a query: ")

# Search for similar embeddings.
db_dir = realpath('./pilots/db')
docs = search_text(query, db_dir, 5)

# Construct the prompt.
head = f"Based on the knowledge from the following document, answer the question \"{query}\"\n\n"
body = ""
for i, doc in enumerate(docs):
  body += f"Document {i + 1}: {docs[i].page_content}\n\n"
prompt = head + body
print(prompt)

# Query LLM for the answer.
