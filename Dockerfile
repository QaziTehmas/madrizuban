FROM node:20-bookworm-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential gcc make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY backend/api/package*.json ./backend/api/
WORKDIR /app/backend/api
RUN npm install --omit=dev

WORKDIR /app
COPY backend/api ./backend/api
COPY backend/compiler ./backend/compiler

WORKDIR /app/backend/compiler
RUN make

WORKDIR /app/backend/api
ENV NODE_ENV=production
ENV PORT=3000
ENV COMPILER_PATH=/app/backend/compiler/madrizuban
EXPOSE 3000
CMD ["npm", "start"]
