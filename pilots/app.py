from utils import embed_text, search_embedding
from os.path import realpath, join

# Ask user for input.
query = input ("Enter a query: ")

# Convert string into embeddings.
query_embedding = embed_text(query)

# Search for similar embeddings.
data_dir = 'data'
db_path = realpath(join(data_dir, 'embeddings.db'))
search_results = search_embedding(query_embedding, db_path)
print(search_results)
